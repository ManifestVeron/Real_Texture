// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvSim_TCPClient.h"

#include "Engine.h"

// Sets default values
AEnvSim_TCPClient::AEnvSim_TCPClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnvSim_TCPClient::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnvSim_TCPClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnvSim_TCPClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Stop_EnvSim_TCPClient();
	Super::EndPlay(EndPlayReason);
}

void AEnvSim_TCPClient::Start_EnvSim_TCPClient
(
	const FString& ipAddress, 
	int32 port,
	const FEnvPacketReceivedDelegate& OnEnvPacketReceived 
)
{
	EnvPacketReceivedDelegate = OnEnvPacketReceived;

	TWeakObjectPtr<AEnvSim_TCPClient> thisWeakObjPtr = TWeakObjectPtr<AEnvSim_TCPClient>(this);
	EnvSim_TCPClientWorker = TUniquePtr<FEnvSim_TCPClientWorker>(new FEnvSim_TCPClientWorker(ipAddress, port, thisWeakObjPtr, TimeBetweenTicks));
	EnvSim_TCPClientWorker->Start();

}

void AEnvSim_TCPClient::Stop_EnvSim_TCPClient()
{
	if(EnvSim_TCPClientWorker)
	{
		UE_LOG(LogTemp, Warning, TEXT("Env Sim TCP Client Stopped"));
		FPlatformTime::Seconds();
		EnvSim_TCPClientWorker->Stop();
	}
}


void AEnvSim_TCPClient::ExecuteOnEnvPacketReceived(TWeakObjectPtr<AEnvSim_TCPClient> thisObj)
{
	if (!thisObj.IsValid())
		return;
	
	EnvPacketReceivedDelegate.ExecuteIfBound(EnvSim_TCPClientWorker->ReadFromInbox());
}


void AEnvSim_TCPClient::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("Env Sim TCP Client Log");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}

//
// Worker Class. TCP Client for interconnation with Environment Simulator
//

FEnvSim_TCPClientWorker::FEnvSim_TCPClientWorker
(
	FString inIp,
	const int32 inPort,
	TWeakObjectPtr<AEnvSim_TCPClient> InOwner, 
	float inTimeBetweenTicks
):
	ipAddress(inIp),
	port(inPort),
	TimeBetweenTicks(inTimeBetweenTicks),
	ThreadSpawnerActor(InOwner)
{

}

FEnvSim_TCPClientWorker::~FEnvSim_TCPClientWorker()
{
	AsyncTask(ENamedThreads::GameThread, []()
	{
		AEnvSim_TCPClient::PrintToConsole("Env Sim TCP Client thread was destroyed.", false);
	});

	Stop();
	if(Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FEnvSim_TCPClientWorker::Start()
{
	check(!Thread && "Env Sim TCP Client thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");
	
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Env Sim TCP Client thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	
	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("FEnvSim_TCPClientWorker")), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Warning, TEXT("Created Env Sim TCP Client thread"));
}

TArray<FTarget> FEnvSim_TCPClientWorker::ReadFromInbox()
{
	TArray<FTarget> targets;
	Inbox.Dequeue(targets);
	return targets;
}

bool FEnvSim_TCPClientWorker::Init()
{
	bRun = true;
	bConnected = false;
	return true;
}

TSharedRef<FInternetAddr> FEnvSim_TCPClientWorker::SocketSetParams()
{
	FIPv4Address ip;
	FIPv4Address::Parse(ipAddress, ip);

	ISocketSubsystem* SocketSybstem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> host = SocketSybstem->CreateInternetAddr();

	host->SetIp(ip.Value);
	host->SetPort(port);

	return host;
}

void FEnvSim_TCPClientWorker::SleepBetweenTicks(FDateTime timeBeginning, FDateTime timeEnd)
{
	FTimespan tickDuration = timeEnd - timeBeginning;
	float secondsThisTickTook = tickDuration.GetTotalSeconds();
	float timeToSleep = TimeBetweenTicks - secondsThisTickTook;
	if (timeToSleep > 0.f)
	{
		FPlatformProcess::Sleep(timeToSleep);
	}
}

uint32 FEnvSim_TCPClientWorker::Run()
{
	AsyncTask(ENamedThreads::GameThread, []()
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting Env Sim TCP Client thread"));
	});	
	
	FDateTime timeBeginningOfTick;
	FDateTime timeEndOfTick;
	int32 BytesRead;
	int32 BytesSent;
	while(bRun)
	{
		ConnectionSocket = FTcpSocketBuilder(TEXT("TCP Connection. Env Sim")).Build();

		if(!ConnectionSocket)
		{
			UE_LOG(LogTemp, Warning, TEXT("WE CAN'T CREATE A TCP SOCKET FOR CONNECTING TO ENVIRONMENT SIMULATOR"));
			FGenericPlatformMisc::RequestExit(false);
		}	

		while(!bConnected  && bRun)
		{

			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.5f, FColor::Red, FString(TEXT("[Env Sim Client] Try to connection ... ")));
			});
			
			ConnectionSocket->Connect(*SocketSetParams());
			ConnectionSocket->SetNonBlocking(true);
			uint8 t_Dummy; 
			bConnected = ConnectionSocket->Recv(&t_Dummy, 1, BytesRead, ESocketReceiveFlags::Peek);
			ConnectionSocket->SetNonBlocking(false);

			if(bConnected)
			{
				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Green, FString(TEXT("[Env Sim Client] Successful connection")));
				});
				break;
			}

			FPlatformProcess::Sleep(0.5f);
		}

		FString cmd(TEXT("cmd"));
		while(bConnected)
		{
			timeBeginningOfTick = FDateTime::UtcNow();
			
			// Cmd send
			const uint8* u_cmd = (uint8*)TCHAR_TO_UTF8(cmd.GetCharArray().GetData());
			ConnectionSocket->Send(u_cmd, cmd.Len(), BytesSent);

			if(BytesSent != cmd.Len())
			{
				bConnected = false;
				break;
			}

			EnvPacket env_packet;
			TArray<FTarget> targets;

			ConnectionSocket->Recv((uint8*)&env_packet, sizeof(EnvPacket), BytesRead);

			if(BytesRead != sizeof(EnvPacket))
			{
				bConnected = false;
			}
			else
			{
				for (int idx = 0; idx < TARGETS_COUNT; ++idx)
				{
					if(env_packet.targets[idx].class_id > -1)
					{
						targets.Add(FTarget(env_packet.targets[idx]));
					}
				}
				
				Inbox.Enqueue(targets);

				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					ThreadSpawnerActor.Get()->ExecuteOnEnvPacketReceived(ThreadSpawnerActor);
				});
			}

			timeEndOfTick = FDateTime::UtcNow();
			SleepBetweenTicks(timeBeginningOfTick, timeEndOfTick);
		}

		FPlatformProcess::Sleep(TimeBetweenTicks);
		Inbox.Empty();

		SocketShutdown();

	}
			
	return 0;
}

void FEnvSim_TCPClientWorker::Stop()
{
	bConnected = false;
	bRun = false;
}

void FEnvSim_TCPClientWorker::SocketShutdown()
{	
	ConnectionSocket->Close();
	delete ConnectionSocket;
	ConnectionSocket = nullptr;
}
