// Fill out your copyright notice in the Description page of Project Settings.

#include "MathModel_TCPClient.h"

#include "Engine.h"

//
// Actor Class. TCP Client for interconnation with Math Model
//

// Sets default values
AMathModel_TCPClient::AMathModel_TCPClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMathModel_TCPClient::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMathModel_TCPClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMathModel_TCPClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Stop_MathModel_TCPClient();
	Super::EndPlay(EndPlayReason);
}

void AMathModel_TCPClient::Start_MathModel_TCPClient
(
	const FString& ipAddress, 
	int32 port,
	const FMathPacketReceivedDelegate& OnMathPacketReceived 
)
{
	MathPacketReceivedDelegate = OnMathPacketReceived;

	TWeakObjectPtr<AMathModel_TCPClient> thisWeakObjPtr = TWeakObjectPtr<AMathModel_TCPClient>(this);
	MathModel_TCPClientWorker = TUniquePtr<FMathModel_TCPClientWorker>(new FMathModel_TCPClientWorker(ipAddress, port, thisWeakObjPtr, TimeBetweenTicks));
	MathModel_TCPClientWorker->Start();

}

void AMathModel_TCPClient::Stop_MathModel_TCPClient()
{
	if(MathModel_TCPClientWorker)
	{
		UE_LOG(LogTemp, Warning, TEXT("Math Model TCP Client Stopped"));
		FPlatformTime::Seconds();
		MathModel_TCPClientWorker->Stop();
	}
}


void AMathModel_TCPClient::ExecuteOnMathPacketReceived(TWeakObjectPtr<AMathModel_TCPClient> thisObj)
{
	if (!thisObj.IsValid())
		return;
	
	while(!MathModel_TCPClientWorker->BufferEmpty())
		MathPacketReceivedDelegate.ExecuteIfBound(MathModel_TCPClientWorker->ReadFromInbox());
}


void AMathModel_TCPClient::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("Math Model TCP Client Log");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}


//
// Worker Class. TCP Client for interconnation with Math Model
//

	FMathModel_TCPClientWorker::FMathModel_TCPClientWorker
	(
		FString inIp,
		const int32 inPort,
		TWeakObjectPtr<AMathModel_TCPClient> InOwner, 
		float inTimeBetweenTicks
	):
	ipAddress(inIp),
	port(inPort),
	TimeBetweenTicks(inTimeBetweenTicks),
	ThreadSpawnerActor(InOwner)
{

}

FMathModel_TCPClientWorker::~FMathModel_TCPClientWorker()
{
	AsyncTask(ENamedThreads::GameThread, []()
	{
		AMathModel_TCPClient::PrintToConsole("Math Model TCP Client thread was destroyed.", false);
	});

	Stop();
	if(Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FMathModel_TCPClientWorker::Start()
{
	check(!Thread && "Math Model TCP Client thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");
	
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Math Model TCP Client thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	
	Thread = FRunnableThread::Create(this, *FString::Printf(TEXT("FMathModel_TCPClientWorker")), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Warning, TEXT("Created Math Model TCP Client thread"));
}

FMathPacket FMathModel_TCPClientWorker::ReadFromInbox()
{
	FMathPacket math_packet;
	Inbox.Dequeue(math_packet);
	return math_packet;
}

bool FMathModel_TCPClientWorker::BufferEmpty()
{
	return Inbox.IsEmpty();
}

bool FMathModel_TCPClientWorker::Init()
{
	bRun = true;
	bConnected = false;
	return true;
}

TSharedRef<FInternetAddr> FMathModel_TCPClientWorker::SocketSetParams()
{
	FIPv4Address ip;
	FIPv4Address::Parse(ipAddress, ip);

	ISocketSubsystem* SocketSybstem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	
	TSharedRef<FInternetAddr> host = SocketSybstem->CreateInternetAddr();
	host->SetIp(ip.Value);
	host->SetPort(port);

	return host;
}

void FMathModel_TCPClientWorker::SleepBetweenTicks(FDateTime timeBeginning, FDateTime timeEnd)
{
	FTimespan tickDuration = timeEnd - timeBeginning;
	float secondsThisTickTook = tickDuration.GetTotalSeconds();
	float timeToSleep = TimeBetweenTicks - secondsThisTickTook;
	if (timeToSleep > 0.f)
	{
		FPlatformProcess::Sleep(timeToSleep);
	}
}

uint32 FMathModel_TCPClientWorker::Run()
{
	AsyncTask(ENamedThreads::GameThread, []()
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting Math Model TCP Client thread"));
	});	
	
	FDateTime timeBeginningOfTick;
	FDateTime timeEndOfTick;
	int32 BytesRead;
	
	// Цикл потока, пока идет симуляция
	while(bRun)
	{
		ConnectionSocket = FTcpSocketBuilder(TEXT("TCP Connection. Math Model"))
			.WithReceiveBufferSize(10*sizeof(MathPacket))
			.Build();

		if(!ConnectionSocket)
		{
			UE_LOG(LogTemp, Warning, TEXT("WE CAN'T CREATE A TCP SOCKET FOR CONNECTING TO MATH MODEL"));
			FGenericPlatformMisc::RequestExit(false);
		}	

		// Цикл для попыток подключения
		while(!bConnected  && bRun)
		{

			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.5f, FColor::Red, 
				FString(TEXT("[Math Model Client] Try to connection ... ")));
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
					GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Green, 
					FString(TEXT("[Math Model Client] Successful connection")));
				});
				break;
			}

			FPlatformProcess::Sleep(0.5f);
		}

		ConnectionSocket->SetNonBlocking(true);
		
		// Цикл соединения
		while (bConnected)
		{
			timeBeginningOfTick = FDateTime::UtcNow();
			
			uint32 pendingDataSize = 0;

			uint8 t_Dummy;
			bConnected = ConnectionSocket->Recv(&t_Dummy, 1, BytesRead, ESocketReceiveFlags::Peek);

			if (!bConnected)
				break;

			ConnectionSocket->HasPendingData(pendingDataSize);
			if (pendingDataSize < sizeof(MathPacket))
			{
				timeEndOfTick = FDateTime::UtcNow();
				SleepBetweenTicks(timeBeginningOfTick, timeEndOfTick);
				continue;
			}
				

			uint8* buffer = new uint8[pendingDataSize];
			uint8 prefix[3] = { 
				static_cast<uint8>(0xBB), 
				static_cast<uint8>(0xAA), 
				static_cast<uint8>(0xCC) 
			};

			int pref_size = sizeof(prefix) / sizeof(uint8);

			// Читаем данные, собранные в буфере сокета
			bConnected = ConnectionSocket->Recv(buffer, pendingDataSize, BytesRead);

			if (!bConnected)
			{
				delete[] buffer;
				buffer = 0;
				break;
			}

			// Ищем пакеты в прочитанном буфере
			int buf_idx = 0;
			int pref_idx = 0;
			
			for (buf_idx = 0; buf_idx <= (BytesRead - pref_size); ++buf_idx)
			{
				for (pref_idx = 0; pref_idx < pref_size && buffer[buf_idx + pref_idx] == prefix[pref_idx]; ++pref_idx);

				if (pref_idx == pref_size && buf_idx <= (BytesRead - sizeof(MathPacket)))
				{
					MathPacket math_packet;
					memcpy_s(&math_packet, sizeof(math_packet), buffer + buf_idx, sizeof(math_packet));
					Inbox.Enqueue(FMathPacket(math_packet));
				}
			}

			delete[] buffer;
			buffer = 0;

			//UE_LOG(LogTemp, Log, TEXT("[Math Client] : After math packets searching"));

			// Вызываем делегат на обработку очереди
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				ThreadSpawnerActor.Get()->ExecuteOnMathPacketReceived(ThreadSpawnerActor);
			});

			timeEndOfTick = FDateTime::UtcNow();
			SleepBetweenTicks(timeBeginningOfTick, timeEndOfTick);
		}

		FPlatformProcess::Sleep(TimeBetweenTicks);
		Inbox.Empty();
		
		SocketShutdown();
	}
			
	return 0;
}

void FMathModel_TCPClientWorker::Stop()
{
	bConnected = false;
	bRun = false;
}

void FMathModel_TCPClientWorker::SocketShutdown()
{	
	ConnectionSocket->Close();
	delete ConnectionSocket;
	ConnectionSocket = nullptr;
}
