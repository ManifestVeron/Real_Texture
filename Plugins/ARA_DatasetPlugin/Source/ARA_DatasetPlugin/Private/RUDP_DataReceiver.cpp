// Fill out your copyright notice in the Description page of Project Settings.


#include "RUDP_DataReceiver.h"


/**
 * 
 */

// Sets default values
ARUDP_DataReceiver::ARUDP_DataReceiver()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARUDP_DataReceiver::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARUDP_DataReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARUDP_DataReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Stop_RUDP_CLO_Receiver();
	Super::EndPlay(EndPlayReason);
}

void ARUDP_DataReceiver::Start_RUDP_CLO_Receiver
(
	const FString& ipAddress, 
	int32 port, 
	const FRUDP_DataReceiverDelegate& OnDataReceived
)
{
	data_received_delegate = OnDataReceived;

	TWeakObjectPtr<ARUDP_DataReceiver> thisWeakObjPtr = TWeakObjectPtr<ARUDP_DataReceiver>(this);
	rudp_data_receiver_worker = TUniquePtr<FRUDP_DataReceiverWorker>(new FRUDP_DataReceiverWorker(ipAddress, port, thisWeakObjPtr, TimeBetweenTicks));
	rudp_data_receiver_worker->Start();
}

void ARUDP_DataReceiver::Stop_RUDP_CLO_Receiver()
{

	if (rudp_data_receiver_worker)
	{
		UE_LOG(LogTemp, Warning, TEXT("RUDP Receiver stopped"));
		rudp_data_receiver_worker->Stop();
	}
}

void ARUDP_DataReceiver::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("RUDP Receiver Log");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}

void ARUDP_DataReceiver::ExecuteOnMessageReceived(TWeakObjectPtr<ARUDP_DataReceiver> thisObj)
{
	if (!thisObj.IsValid())
		return;

	TTuple<CamPose, EnvironmentData, TArray<ObjectPose>> Positions = rudp_data_receiver_worker->ReadFromInbox();

	data_received_delegate.ExecuteIfBound
	(
		FCamPose(Positions.Get<0>()), 
		FEnvironmentData(Positions.Get<1>()),
		TArray<FObjectPose>(Positions.Get<2>())
	);

}

/**
*
*/

FRUDP_DataReceiverWorker::FRUDP_DataReceiverWorker
(
	FString inIp, 
	const int32 inPort, 
	TWeakObjectPtr<ARUDP_DataReceiver> InOwner, 
	float inTimeBetweenTicks
) :
	ipAddress(inIp),
	port(inPort),
	ThreadSpawnerActor(InOwner),
	TimeBetweenTicks(inTimeBetweenTicks)
{

}

FRUDP_DataReceiverWorker::~FRUDP_DataReceiverWorker()
{
	AsyncTask(ENamedThreads::GameThread, []() 
	{
		ARUDP_DataReceiver::PrintToConsole("RUDP DataReceiverWorker was destroyed.", false);
	});

	Stop();

	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FRUDP_DataReceiverWorker::Start()
{
	check(!Thread && "Thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("FRUDP_DataReceiverWorker %s:%d"), *ipAddress, port), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Warning, TEXT("Created RUDP DataReceiver thread"));
}

TTuple<CamPose, EnvironmentData, TArray<ObjectPose>> FRUDP_DataReceiverWorker::ReadFromInbox()
{
	TTuple<CamPose, EnvironmentData, TArray<ObjectPose>> Data;
	Inbox.Dequeue(Data);
	return Data;
}

bool FRUDP_DataReceiverWorker::Init()
{
	bRun = true;
	return true;
}

uint32 FRUDP_DataReceiverWorker::Run()
{
	FIPv4Address ip;
	FIPv4Address::Parse(ipAddress, ip);
	const FIPv4Endpoint Endpoint(ip, port);
	ConnectionSocket = FUdpSocketBuilder(TEXT("UDP Connection. RUDP Receiver"))
		.AsBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Build();

	if (!ConnectionSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("WE CAN'T CREATE A UDP SOCKET for RUDP Receiver"));
		FGenericPlatformMisc::RequestExit(false);
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	UE_LOG(LogTemp, Warning, TEXT("Starting RUDP DataReceiver thread."));

	FString OK_message = TEXT("OK");

	CamPose			cam_packet;
	EnvironmentData	env_packet;
	ObjectsNum		obj_num_packet;
	ObjectPose		object_packet;

	int32 BufferSize;

	bool bRecvStatus;
	bool bSentStatus;

	while (bRun)
	{

		FDateTime timeBeginningOfTick = FDateTime::UtcNow();

		int32 BytesRead = 0;
		int32 BytesSent = 0;

		// camera
		BufferSize = sizeof(CamPose);

		ConnectionSocket->SetNonBlocking(true);
		bRecvStatus = ConnectionSocket->RecvFrom((uint8*)&cam_packet, BufferSize, BytesRead, *Addr);

		if (!bRecvStatus && BytesRead == 0)
		{
			FDateTime timeEndOfTick = FDateTime::UtcNow();
			FTimespan tickDuration = timeEndOfTick - timeBeginningOfTick;
			float secondsThisTickTook = tickDuration.GetTotalSeconds();
			float timeToSleep = TimeBetweenTicks - secondsThisTickTook;

			if (timeToSleep > 0.f)
			{
				FPlatformProcess::Sleep(timeToSleep);
			}

			continue;
		}

		ConnectionSocket->SetNonBlocking(false);

		UE_LOG(LogTemp, Log, TEXT("++++ SERVER LOG ++++"));

		UE_LOG(LogTemp, Log, TEXT("CAM PACKET INDEX-- %d"), cam_packet.index);
		UE_LOG(LogTemp, Log, TEXT("---------------------"));

		bSentStatus = ConnectionSocket->SendTo((uint8*)TCHAR_TO_UTF8(OK_message.GetCharArray().GetData()), OK_message.GetCharArray().Num() - 1, BytesSent, *Addr);
		if (!bSentStatus && BytesSent == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("FAILED TO SENT ANSWER (CAMERA POSE)"));
			bRun = false;
			continue;
		}

		// Environment Data
		BufferSize = sizeof(EnvironmentData);

		bRecvStatus = ConnectionSocket->RecvFrom((uint8*)&env_packet, BufferSize, BytesRead, *Addr);
		if (!bRecvStatus && BytesRead == 0)
		{
			bRun = false;
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("Environment PACKET INDEX -- %d"), env_packet.index);
		UE_LOG(LogTemp, Log, TEXT("---------------------"));

		bSentStatus = ConnectionSocket->SendTo((uint8*)TCHAR_TO_UTF8(OK_message.GetCharArray().GetData()), OK_message.GetCharArray().Num() - 1, BytesSent, *Addr);
		if (!bSentStatus && BytesSent == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("FAILED TO SENT ANSWER (LIGHTING DATA)"));
			bRun = false;
			continue;
		}

		// objects num
		BufferSize = sizeof(ObjectsNum);

		bRecvStatus = ConnectionSocket->RecvFrom((uint8*)&obj_num_packet, BufferSize, BytesRead, *Addr);
		if (!bRecvStatus && BytesRead == 0)
		{
			bRun = false;
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("OBJECTS NUM PACKET INDEX -- %d"), obj_num_packet.index);
		UE_LOG(LogTemp, Log, TEXT("OBJECTS NUM -- %d"), obj_num_packet.num);
		UE_LOG(LogTemp, Log, TEXT("---------------------"));

		bSentStatus = ConnectionSocket->SendTo((uint8*)TCHAR_TO_UTF8(OK_message.GetCharArray().GetData()), OK_message.GetCharArray().Num() - 1, BytesSent, *Addr);
		if (!bSentStatus && BytesSent == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("FAILED TO SENT ANSWER (OBJECT POSE)"));
			bRun = false;
			continue;
		}

		// objects
		BufferSize = sizeof(ObjectPose);
		bool bBadRecv = false;
		bool bBadSend = false;

		objectPoses.Empty();

		for (int i = 0; i < obj_num_packet.num; ++i)
		{
			bRecvStatus = ConnectionSocket->RecvFrom((uint8*)&object_packet, BufferSize, BytesRead, *Addr);
			if (!bRecvStatus && BytesRead == 0)
			{
				UE_LOG(LogTemp, Log, TEXT("FAILED RECEIVED OBJECT PACKET"));
				bBadRecv = true;
				break;
			}

			UE_LOG(LogTemp, Log, TEXT("OBJ PACKET INDEX -- %d "), object_packet.index);
			UE_LOG(LogTemp, Log, TEXT("OBJ PLACE -- #%d "), object_packet.place_id);
			UE_LOG(LogTemp, Log, TEXT("--"));

			bSentStatus = ConnectionSocket->SendTo((uint8*)TCHAR_TO_UTF8(OK_message.GetCharArray().GetData()), OK_message.GetCharArray().Num() - 1, BytesSent, *Addr);
			if (!bSentStatus && BytesSent == 0)
			{
				UE_LOG(LogTemp, Log, TEXT("FAILED TO SENT ANSWER (OBJECT PACKET)"));
				bBadSend = true;
				break;
			}
			if (obj_num_packet.index != object_packet.index)
			{
				UE_LOG(LogTemp, Log, TEXT("RECEIVED PACKET OF WRONG INDEX"));
				bBadRecv = true;
				break;
			}

			objectPoses.Add(object_packet);
		}
		UE_LOG(LogTemp, Log, TEXT("---------------------"));

		if (bBadRecv)
			continue;
		if (bBadSend)
			break;

		if (cam_packet.index == env_packet.index && cam_packet.index == object_packet.index)
		{
			Inbox.Enqueue(MakeTuple(cam_packet, env_packet, objectPoses));

			UE_LOG(LogTemp, Log, TEXT(">> ALL DATA (CAM, Environment, NUM OBJS, OBJS) INDEX -- %d <<"), cam_packet.index);

			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				ThreadSpawnerActor.Get()->ExecuteOnMessageReceived(ThreadSpawnerActor);
			});
		}
	}
	SocketShutdown();

	return 0;
}

void FRUDP_DataReceiverWorker::Stop()
{
	bRun = false;
}

void FRUDP_DataReceiverWorker::SocketShutdown()
{

	UE_LOG(LogTemp, Warning, TEXT("Socket Shutdown"));

	ConnectionSocket->Close();
	delete ConnectionSocket;
	ConnectionSocket = nullptr;
}
