// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvPacket.h"

#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "HAL/RunnableThread.h"

#include "Async/Async.h"
#include "Logging/MessageLog.h"
#include "Containers/Queue.h"
//#include "SizedQueue.h"
#include "Uobject/WeakObjectPtrTemplates.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Common/TcpSocketBuilder.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvSim_TCPClient.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FEnvPacketReceivedDelegate, UPARAM(ref) TArray<FTarget>, targets);

//
//	Actor Class. TCP Client for interconnection with Environment Simulator
//
UCLASS()
class ARA_NETWORKS_API AEnvSim_TCPClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvSim_TCPClient();

	UFUNCTION(BlueprintCallable, Category="ARA Networks | TCP Client for Environment Simulator")
		void Start_EnvSim_TCPClient
		(
			const FString& ipAddress, 
			int32 port,
			const FEnvPacketReceivedDelegate& OnEnvPacketReceived 
		);
	
	UFUNCTION(BlueprintCallable, Category="ARA Networks | TCP Client for Environment Simulator")
		void Stop_EnvSim_TCPClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ExecuteOnEnvPacketReceived(TWeakObjectPtr<AEnvSim_TCPClient> thisObj);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARA Networks | TCP Client for Environment Simulator")
		float TimeBetweenTicks = 0.01f;
	
	static void PrintToConsole(FString Str, bool Error);

private:
	TUniquePtr<class FEnvSim_TCPClientWorker> EnvSim_TCPClientWorker;
	FEnvPacketReceivedDelegate EnvPacketReceivedDelegate;

};


//
// Worker Class. TCP Client for interconnation with Environment Simulator
//

class FEnvSim_TCPClientWorker: public FRunnable, public TSharedFromThis<FEnvSim_TCPClientWorker>
{
	FRunnableThread* Thread = nullptr;

private:
	FString ipAddress;
	int32 port;
	float TimeBetweenTicks;

	TQueue<TArray<FTarget>, EQueueMode::Spsc> Inbox;
	//SizedQueue<TArray<FTarget>> Inbox;

	TWeakObjectPtr<AEnvSim_TCPClient> ThreadSpawnerActor;

public:
	class FSocket* ConnectionSocket;

	// Constructor
	FEnvSim_TCPClientWorker
	(
		FString inIp,
		const int32 inPort,
		TWeakObjectPtr<AEnvSim_TCPClient> InOwner, 
		float inTimeBetweenTicks
	);

	// Destructor
	virtual ~FEnvSim_TCPClientWorker();

	// Start processing
	void Start();

	TArray<FTarget> ReadFromInbox();

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
