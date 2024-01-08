// Fill out your copyright notice in the Description page of Project Settings.

#include "UDP_IPoint_Peeper.h"

#include <string>


/**
 *  Actor Class.
 */

 // Sets default values
AUDP_IPoint_Peeper::AUDP_IPoint_Peeper()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AUDP_IPoint_Peeper::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AUDP_IPoint_Peeper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUDP_IPoint_Peeper::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Stop_UDP_IPoint_Peeper();
	Super::EndPlay(EndPlayReason);
}

void AUDP_IPoint_Peeper::Start_UDP_IPoint_Peeper
(
	const FString& ipAddress,
	int32 port,
	const FARA_RecvInterestPointDelegate& OnInterestedPointReceivedDelegate
)
{
	RecvInterestPointDelegate = OnInterestedPointReceivedDelegate;

	TWeakObjectPtr<AUDP_IPoint_Peeper> thisWeakObjPtr = TWeakObjectPtr<AUDP_IPoint_Peeper>(this);
	IPointPeeperWorker = TUniquePtr<FUDP_IPointPeeperWorker>(new FUDP_IPointPeeperWorker(ipAddress, port, thisWeakObjPtr, TimeBetweenTicks));
	IPointPeeperWorker->Start();

	UE_LOG(LogTemp, Warning, TEXT("ARA UDP IPeeper Started"));
}

void AUDP_IPoint_Peeper::Stop_UDP_IPoint_Peeper()
{
	if (IPointPeeperWorker)
	{
		UE_LOG(LogTemp, Warning, TEXT("ARA UDP IPeeper Stopped"));
		IPointPeeperWorker->Stop();
	}
}

void AUDP_IPoint_Peeper::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("ARA UDP IPeeper");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}

void AUDP_IPoint_Peeper::ExecuteOnInterestPointReceived(TWeakObjectPtr<AUDP_IPoint_Peeper> thisObj)
{
	
	if (!thisObj.IsValid())
		return;

	FARA_InterestPoint ipoint = IPointPeeperWorker->ReadFromInbox();

	RecvInterestPointDelegate.ExecuteIfBound(ipoint);
}

void AUDP_IPoint_Peeper::SendIPointData(FARA_InterestPoint v_ipoint)
{
	IPointPeeperWorker->AddToOutbox(v_ipoint);
}

/*
*  Worker Class.
*/

FUDP_IPointPeeperWorker::FUDP_IPointPeeperWorker
(
	FString inIp,
	const int32 inPort,
	TWeakObjectPtr<AUDP_IPoint_Peeper> InOwner,
	float inTimeBetweenTicks
) :
	ipAddress(inIp),
	port(inPort),
	ThreadSpawnerActor(InOwner),
	TimeBetweenTicks(inTimeBetweenTicks)
{
}

FUDP_IPointPeeperWorker::~FUDP_IPointPeeperWorker()
{
	AsyncTask(ENamedThreads::GameThread, []()
	{
		AUDP_IPoint_Peeper::PrintToConsole("ARA UDP IPeeper thread was destroyed.", false);
	});

	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FUDP_IPointPeeperWorker::Start()
{
	check(!Thread && "Thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");

	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Thread isn't null. It's: %s"), *Thread->GetThreadName());
	}

	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("ARA UDP IPeeper thread %s:%d"), *ipAddress, port), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Warning, TEXT("Created ARA UDP IPeeper thread"));
}

bool FUDP_IPointPeeperWorker::Init()
{
	bRun = true;
	return true;
}

uint32 FUDP_IPointPeeperWorker::Run()
{
	FIPv4Address ip;
	FIPv4Address::Parse(ipAddress, ip);

	FIPv4Address ipAny;
	FIPv4Address::Parse(TEXT("0.0.0.0"), ipAny);

	FIPv4Endpoint Endpoint(ip, port);
	RecvConnectionSocket = FUdpSocketBuilder(TEXT("UDP Connection. ARA UDP IPeeper"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToAddress(ipAny)
		.BoundToPort(port)
		.JoinedToGroup(ip)
		.WithMulticastInterface(ip)
		.WithMulticastLoopback()
		.Build();

	if (!RecvConnectionSocket)
	{
		return 0;
	}

	AsyncTask(ENamedThreads::GameThread, []()
	{
		AUDP_IPoint_Peeper::PrintToConsole("Starting ARA UDP IPeeper thread.", false);
	});

	const int buff_size = 1024;
	uint8* buffer = new uint8[buff_size];

	while (bRun)
	{
		FDateTime timeBeginningOfTick = FDateTime::UtcNow();
		
		memset(buffer, 0, buff_size);
		int32 BytesRead = 0;
		if
		(
			RecvConnectionSocket->RecvFrom(buffer, buff_size, BytesRead, *(Endpoint.ToInternetAddr())) &&
			BytesRead != 0
		)
		{
			//UE_LOG(LogTemp, Log, TEXT("GET PACKET FROM BOARD"));

			nlohmann::json j_data = nlohmann::json::parse(buffer);
			std::string command(j_data["command"]);
			if (command == "setPosition")
			{
				control_ipoint = FARA_InterestPoint
				(
					(double)j_data["lat"],
					(double)j_data["lon"],
					(double)j_data["alt"]
				);

				Inbox.Enqueue(control_ipoint);
				control_json = j_data;

				UE_LOG(LogTemp, Log, TEXT("INTERST POINT WAS SET"));

				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					ThreadSpawnerActor.Get()->ExecuteOnInterestPointReceived(ThreadSpawnerActor);
				});
			}
		}

		if (!OutBox.IsEmpty())
		{
			FARA_InterestPoint current_point;
			OutBox.Dequeue(current_point);

			if (fabs(control_ipoint.alt.convert_to_double() - current_point.alt.convert_to_double()) > 0.01)
			{
				control_json["lat"] = current_point.lat.convert_to_double();
				control_json["lon"] = current_point.lon.convert_to_double();
				control_json["alt"] = current_point.alt.convert_to_double();

				FString answer(control_json.dump().c_str());

				int32 BytesSend = 0;

				RecvConnectionSocket->SendTo((uint8*)TCHAR_TO_UTF8(answer.GetCharArray().GetData()),
					answer.Len(), BytesSend, *(Endpoint.ToInternetAddr()));

				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					UE_LOG(LogTemp, Log, TEXT("CORRECT ALTITUDE OF INTEREST POINT WAS SENT"));
				});
			}
		}

		/* In order to sleep, we will account for how much this tick took due to sending and receiving */
		FDateTime timeEndOfTick = FDateTime::UtcNow();
		FTimespan tickDuration = timeEndOfTick - timeBeginningOfTick;
		float secondsThisTickTook = tickDuration.GetTotalSeconds();
		float timeToSleep = TimeBetweenTicks - secondsThisTickTook;
		if (timeToSleep > 0.f)
		{
			FPlatformProcess::Sleep(timeToSleep);
		}
	}

	SocketShutdown();

	return 0;
}

FARA_InterestPoint FUDP_IPointPeeperWorker::ReadFromInbox()
{
	FARA_InterestPoint ipoint;
	Inbox.Dequeue(ipoint);
	return ipoint;
}

void FUDP_IPointPeeperWorker::AddToOutbox(FARA_InterestPoint v_ipoint)
{
	OutBox.Enqueue(v_ipoint);
}

void FUDP_IPointPeeperWorker::Stop()
{
	bRun = false;
}

void FUDP_IPointPeeperWorker::Exit()
{

}

void FUDP_IPointPeeperWorker::SocketShutdown()
{

	RecvConnectionSocket->Close();
	delete RecvConnectionSocket;
	RecvConnectionSocket = nullptr;
}

