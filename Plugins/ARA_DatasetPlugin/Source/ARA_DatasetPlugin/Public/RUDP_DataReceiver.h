// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CamPose.h"
#include "EnvironmentData.h"
#include "ObjectPose.h"

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "IPAddress.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Common/UdpSocketBuilder.h"

#include "Async/Async.h"
#include "Logging/MessageLog.h"
#include "EngineUtils.h"
#include "Kismet/KismetStringLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RUDP_DataReceiver.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FRUDP_DataReceiverDelegate, UPARAM(ref) FCamPose, CameraPose, UPARAM(ref) FEnvironmentData, Light, UPARAM(ref) TArray<FObjectPose>, ObjectsPoses);


UCLASS()
class ARA_DATASETPLUGIN_API ARUDP_DataReceiver : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARUDP_DataReceiver();

	/* Returns the ID of the new connection. */
	UFUNCTION(BlueprintCallable, Category = "RUDP Receiver | Camera, Light, Objects")
		void Start_RUDP_CLO_Receiver
		(
			const FString& ipAddress, 
			int32 port,
			const FRUDP_DataReceiverDelegate& OnDataReceived
		);

	/* Disconnect from connection ID. */
	UFUNCTION(BlueprintCallable, Category = "RUDP Receiver | Camera, Light, Objects")
		void Stop_RUDP_CLO_Receiver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ExecuteOnMessageReceived(TWeakObjectPtr<ARUDP_DataReceiver> thisObj);


	/* Used by the separate threads to print to console on the main thread. */
	static void PrintToConsole(FString Str, bool Error);

	/* Time between ticks. Please account for the fact that it takes 1ms to wake up on a modern PC, so 0.01f would effectively be 0.011f */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time between ticks")
		float TimeBetweenTicks = 0.0009f;

private:
	TUniquePtr<class FRUDP_DataReceiverWorker> rudp_data_receiver_worker;
	FRUDP_DataReceiverDelegate data_received_delegate;

};

class FRUDP_DataReceiverWorker : public FRunnable, public TSharedFromThis<FRUDP_DataReceiverWorker>
{

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread = nullptr;

private:
	FString ipAddress;
	int port;
	TWeakObjectPtr<ARUDP_DataReceiver> ThreadSpawnerActor;
	int32 RecvBufferSize;
	int32 ActualRecvBufferSize;
	float TimeBetweenTicks;

	TArray<ObjectPose> objectPoses;

	// SPSC = single producer, single consumer.
	TQueue<TTuple<CamPose, EnvironmentData, TArray<ObjectPose>>, EQueueMode::Spsc> Inbox;

public:
	class FSocket* ConnectionSocket;

	//
	FRUDP_DataReceiverWorker
	(
		FString inIp, 
		const int32 inPort, 
		TWeakObjectPtr<ARUDP_DataReceiver> InOwner, 
		float inTimeBetweenTicks
	);
	// 
	virtual ~FRUDP_DataReceiverWorker();

	/*  Starts processing of the connection. Needs to be called immediately after construction	 */
	void Start();

	/* Reads a message from the inbox queue */
	TTuple<CamPose, EnvironmentData, TArray<ObjectPose>> ReadFromInbox();

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable interface	

	/** Shuts down the thread */
	void SocketShutdown();

private:

	/** thread should continue running */
	FThreadSafeBool bRun = false;
};
