// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ReadySignal.h"

#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "Async/Async.h"
#include "IPAddress.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Logging/MessageLog.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Common/UdpSocketBuilder.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReadySig_UDPSender.generated.h"

/**
 *  Actor Class. UDP Ready Signal Sender for interconnation with Math Model
 */
UCLASS()
class ARA_NETWORKS_API AReadySig_UDPSender : public AActor
{
	GENERATED_BODY()

public:
	AReadySig_UDPSender();
	
	UFUNCTION(BlueprintCallable, Category = "ARA Networks | UDP Ready Signal Sender")
		void Start_ReadySig_UDPSender(const FString& ipADdress, int32 port=30001);

	UFUNCTION(BlueprintCallable, Category = "ARA Networks | UDP Ready Signal Sender")
		void Stop_ReadySig_UDPSender();

	UFUNCTION(BlueprintCallable, Category = "ARA Networks | UDP Ready Signal Sender")
		void Send_ReadySignal(FReadySignal fready_signal);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	static void PrintToConsole(FString Str, bool Error);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARA Networks | UDP Ready Signal Sender")
		float TimeBetweenTicks = 0.0009f;

private:
	TUniquePtr<class FReadySig_UDPSenderWorker> ReadySig_UDPSenderWorker;

};

/*
*  Worker Class. UDP Ready Signal Sender for interconnation with Math Model
*/
class FReadySig_UDPSenderWorker : public FRunnable, public TSharedFromThis<AReadySig_UDPSender>
{
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread = nullptr;

private:
	FSocket* ConnectionSocket;
	FString ipAddress;
	int port;
	TWeakObjectPtr<AReadySig_UDPSender> ThreadSpawnerActor;
	float TimeBetweenTicks;

	TQueue<ReadySignal, EQueueMode::Spsc> OutBox; 

public:

	FReadySig_UDPSenderWorker
	(
		FString inIp,
		const int32 inPort,
		TWeakObjectPtr<AReadySig_UDPSender> InOwner,
		float inTimeBetweenTicks
	);

	virtual ~FReadySig_UDPSenderWorker();

	/*  Starts processing of the connection. Needs to be called immediately after construction	 */
	void Start();

	/* Adds a message to the outgoing message queue */
	void AddToOutbox(ReadySignal ready_signal);

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	// End FRunnable interface	

	/** Shuts down the thread */
	void SocketShutdown();

private:

	/** thread should continue running */
	FThreadSafeBool bRun = false;
};
