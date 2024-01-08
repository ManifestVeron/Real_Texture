// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadySig_UDPSender.h"


/**
 *  Actor Class. UDP Ready Signal Sender for interconnation with Math Model
 */

// Sets default values
AReadySig_UDPSender::AReadySig_UDPSender()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AReadySig_UDPSender::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AReadySig_UDPSender::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AReadySig_UDPSender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Stop_ReadySig_UDPSender();
	Super::EndPlay(EndPlayReason);
}

void AReadySig_UDPSender::Start_ReadySig_UDPSender(const FString& ipAddress, int32 port)
{
	TWeakObjectPtr<AReadySig_UDPSender> thisWeakObjPtr = TWeakObjectPtr<AReadySig_UDPSender>(this);
	ReadySig_UDPSenderWorker = TUniquePtr<FReadySig_UDPSenderWorker>(new FReadySig_UDPSenderWorker(ipAddress, port, thisWeakObjPtr, TimeBetweenTicks));
	ReadySig_UDPSenderWorker->Start();

	UE_LOG(LogTemp, Warning, TEXT("ARA UDP Ready Signal Sender Started"));
}

void AReadySig_UDPSender::Stop_ReadySig_UDPSender()
{
	if (ReadySig_UDPSenderWorker)
	{
		UE_LOG(LogTemp, Warning, TEXT("ARA UDP Ready Signal Sender Stopped"));
		ReadySig_UDPSenderWorker->Stop();
	}
}

void AReadySig_UDPSender::Send_ReadySignal(FReadySignal fready_signal)
{
	ReadySig_UDPSenderWorker->AddToOutbox(fready_signal.GetRawStruct());
}

void AReadySig_UDPSender::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("ARA UDP Sender");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}

/*
*  Worker Class. UDP Ready Signal Sender for interconnation with Math Model
*/

FReadySig_UDPSenderWorker::FReadySig_UDPSenderWorker
(
	FString inIp,
	const int32 inPort,
	TWeakObjectPtr<AReadySig_UDPSender> InOwner,
	float inTimeBetweenTicks
) :
	ipAddress(inIp),
	port(inPort),
	ThreadSpawnerActor(InOwner),
	TimeBetweenTicks(inTimeBetweenTicks)
{
}

FReadySig_UDPSenderWorker::~FReadySig_UDPSenderWorker()
{
	AsyncTask(ENamedThreads::GameThread, []()
	{
		AReadySig_UDPSender::PrintToConsole("ARA UDP Ready Signal Sender thread was destroyed.", false);
	});

	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FReadySig_UDPSenderWorker::Start()
{
	check(!Thread && "Thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");
	
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	
	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("ARA UDP Ready Signal Sender thread %s:%d"), *ipAddress, port), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Warning, TEXT("Created ARA UDP Ready Signal Sender thread"));
}

void FReadySig_UDPSenderWorker::AddToOutbox(ReadySignal ready_signal)
{
	OutBox.Enqueue(ready_signal);
}

bool FReadySig_UDPSenderWorker::Init()
{
	bRun = true;
	return true;
}

uint32 FReadySig_UDPSenderWorker::Run()
{
	FIPv4Address ip;
	FIPv4Address::Parse(ipAddress, ip);
	FIPv4Endpoint Endpoint(ip, port);
	ConnectionSocket = FUdpSocketBuilder(TEXT("UDP Connection. ARA UDP Ready Signal Sender")).AsNonBlocking().AsReusable();

	if (!ConnectionSocket)
	{
		return 0;
	}

	AsyncTask(ENamedThreads::GameThread, []()
	{
		AReadySig_UDPSender::PrintToConsole("Starting ARA UDP Ready Signal Sender thread.", false);
	});

	ConnectionSocket->SetNonBlocking(false);

	while (bRun)
	{
		FDateTime timeBeginningOfTick = FDateTime::UtcNow();

		if (!OutBox.IsEmpty())
		{
			ReadySignal sendData;
			OutBox.Dequeue(sendData);

			int32 BytesSend = 0;
			ConnectionSocket->SendTo((uint8*)&sendData, sizeof(ReadySignal), BytesSend, *(Endpoint.ToInternetAddr()));

			AsyncTask(ENamedThreads::GameThread, [this, sendData]()
			{
				UE_LOG(LogTemp, Log, TEXT("READY SIGNAL SENT. TICK #%lld"), sendData.n_tick);
			});
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

void FReadySig_UDPSenderWorker::Stop()
{
	bRun = false;
}

void FReadySig_UDPSenderWorker::Exit()
{

}

void FReadySig_UDPSenderWorker::SocketShutdown()
{

	ConnectionSocket->Close();
	delete ConnectionSocket;
	ConnectionSocket = nullptr;
}