// Fill out your copyright notice in the Description page of Project Settings.


#include "UDP_DataReceiver.h"

/**
 * 
 */

// Sets default values
AUDP_DataReceiver::AUDP_DataReceiver()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AUDP_DataReceiver::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AUDP_DataReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUDP_DataReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Stop_UDP_CL_Receiver();
	Super::EndPlay(EndPlayReason);
}

void AUDP_DataReceiver::Start_UDP_CL_Receiver
(
	const FString& ipAddress, 
	int32 port, 
	const FUDP_DataReceiverDelegate& OnCameraAndLightStructReceived
)
{
	data_received_delegate = OnCameraAndLightStructReceived;

	TWeakObjectPtr<AUDP_DataReceiver> thisWeakObjPtr = TWeakObjectPtr<AUDP_DataReceiver>(this);
	udp_data_receiver_worker = TUniquePtr<FUDP_DataReceiverWorker>(new FUDP_DataReceiverWorker(ipAddress, port, thisWeakObjPtr, ReceiveBufferSize, TimeBetweenTicks));
	udp_data_receiver_worker->Start();
}

void AUDP_DataReceiver::Stop_UDP_CL_Receiver()
{

	if (udp_data_receiver_worker)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDP CamAndLignt struct receiver stopped"));
		udp_data_receiver_worker->Stop();
	}
}

void AUDP_DataReceiver::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("UDP CamAndLignt struct Receiver Log");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}


void AUDP_DataReceiver::ExecuteOnMessageReceived(TWeakObjectPtr<AUDP_DataReceiver> thisObj)
{

	// the second check is for when we quit PIE, we may get a message about a disconnect, but it's too late to act on it, because the thread has already been killed
	if (!thisObj.IsValid())
		return;

	// how to crash:
	// 1 connect with both clients
	// 2 stop PIE
	// 3 close editor

	data_received_delegate.ExecuteIfBound(FCamAndLight(udp_data_receiver_worker->ReadFromInbox()));

}

/**
 * 
 */

FUDP_DataReceiverWorker::FUDP_DataReceiverWorker
(
	FString inIp, 
	const int32 inPort, 
	TWeakObjectPtr<AUDP_DataReceiver> InOwner, 
	int32 inRecvBufferSize, 
	float inTimeBetweenTicks
)
	: ipAddress(inIp)
	, port(inPort)
	, ThreadSpawnerActor(InOwner)
	, RecvBufferSize(inRecvBufferSize)
	, TimeBetweenTicks(inTimeBetweenTicks)
{

}

FUDP_DataReceiverWorker::~FUDP_DataReceiverWorker()
{
	AsyncTask(ENamedThreads::GameThread, []() 
	{	
		AUDP_DataReceiver::PrintToConsole("UDP DataReceiverWorker was destroyed", false); 
	});

	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FUDP_DataReceiverWorker::Start()
{
	check(!Thread && "Thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("FUDP_DataReceiverWorker %s:%d"), *ipAddress, port), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Warning, TEXT("Created UDP DataReceiver thread"));
}

CamAndLight FUDP_DataReceiverWorker::ReadFromInbox()
{
	CamAndLight Data;
	Inbox.Dequeue(Data);
	return Data;
}

bool FUDP_DataReceiverWorker::Init()
{
	bRun = true;
	return true;
}

uint32 FUDP_DataReceiverWorker::Run()
{
	FIPv4Address ip;
	FIPv4Address::Parse(ipAddress, ip);
	FIPv4Endpoint Endpoint(ip, port);
	ConnectionSocket = FUdpSocketBuilder(TEXT("UDP Connection. UDP Receiver"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(RecvBufferSize);


	if (!ConnectionSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("WE CAN'T CREATE A UDP SOCKET for UDP Receiver"));
		FGenericPlatformMisc::RequestExit(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Starting UDP DataReceiver thread"));
	
	ConnectionSocket->SetReceiveBufferSize(RecvBufferSize, ActualRecvBufferSize);
	ConnectionSocket->SetNonBlocking();

	while (bRun)
	{
		FDateTime timeBeginningOfTick = FDateTime::UtcNow();

		// read bytes as camera pose struct (see CamPose.h)
		CamAndLight receivedData;
		int32 BytesRead = 0;

		if (ConnectionSocket->RecvFrom((uint8*)&receivedData, sizeof(CamAndLight), BytesRead, *(Endpoint.ToInternetAddr())) && BytesRead != 0)
		{
			Inbox.Enqueue(receivedData);
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				UE_LOG(LogTemp, Log, TEXT("RECEIVED CAMERA AND LIGHT DATA STRUCT"));
				ThreadSpawnerActor.Get()->ExecuteOnMessageReceived(ThreadSpawnerActor);
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

void FUDP_DataReceiverWorker::Stop()
{
	bRun = false;
}

void FUDP_DataReceiverWorker::Exit()
{

}

void FUDP_DataReceiverWorker::SocketShutdown()
{
	// if there is still a socket, close it so our peer will get a quick disconnect notification
	//UE_LOG(LogTemp, Warning, TEXT("Socket Shutdown"));
	ConnectionSocket->Close();
	delete ConnectionSocket;
	ConnectionSocket = nullptr;
}


