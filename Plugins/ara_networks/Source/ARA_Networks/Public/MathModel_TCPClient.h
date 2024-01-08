// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MathPacket.h"

#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "HAL/RunnableThread.h"

#include "Async/Async.h"
#include "Logging/MessageLog.h"
#include "Containers/Queue.h"
#include "Uobject/WeakObjectPtrTemplates.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Common/TcpSocketBuilder.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MathModel_TCPClient.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FMathPacketReceivedDelegate, UPARAM(ref) FMathPacket, math_packet);

//
// Actor Class. TCP Client for interconnation with Math Model
//
UCLASS()
class ARA_NETWORKS_API AMathModel_TCPClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMathModel_TCPClient();

	UFUNCTION(BlueprintCallable, Category="ARA Networks | TCP Client for Math Model")
		void Start_MathModel_TCPClient
		(
			const FString& ipAddress, 
			int32 port,
			const FMathPacketReceivedDelegate& OnMathPacketReceived 
		);
	
	UFUNCTION(BlueprintCallable, Category="ARA Networks | TCP Client for Math Model")
		void Stop_MathModel_TCPClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ExecuteOnMathPacketReceived(TWeakObjectPtr<AMathModel_TCPClient> thisObj);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARA Networks | TCP Client for Math Model")
		float TimeBetweenTicks = 0.001f;
	
	static void PrintToConsole(FString Str, bool Error);

private:
	TUniquePtr<class FMathModel_TCPClientWorker> MathModel_TCPClientWorker;
	FMathPacketReceivedDelegate MathPacketReceivedDelegate;
};

//
// Worker Class. TCP Client for interconnation with Math Model
//

class FMathModel_TCPClientWorker: public FRunnable, public TSharedFromThis<FMathModel_TCPClientWorker>
{
	FRunnableThread* Thread = nullptr;

private:
	FString ipAddress;
	int32 port;
	float TimeBetweenTicks;

	TQueue<FMathPacket, EQueueMode::Spsc> Inbox;

	TWeakObjectPtr<AMathModel_TCPClient> ThreadSpawnerActor;

public:
	class FSocket* ConnectionSocket;

	// Constructor
	FMathModel_TCPClientWorker
	(
		FString inIp,
		const int32 inPort,
		TWeakObjectPtr<AMathModel_TCPClient> InOwner, 
		float inTimeBetweenTicks
	);

	// Destructor
	virtual ~FMathModel_TCPClientWorker();

	// Start processing
	void Start();

	FMathPacket ReadFromInbox();
	bool BufferEmpty();

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable interface

	TSharedRef<FInternetAddr> SocketSetParams();
	void SocketShutdown();

private:

	FThreadSafeBool bRun 		= false;
	FThreadSafeBool bConnected 	= false;
	FCriticalSection m_mutex;

	void SleepBetweenTicks(FDateTime timeBeginning, FDateTime timeEnd);
};
