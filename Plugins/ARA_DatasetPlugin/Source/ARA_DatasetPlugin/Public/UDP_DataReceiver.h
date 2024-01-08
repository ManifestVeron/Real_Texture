// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CamAndLight.h"

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "Async/Async.h"
#include "Logging/MessageLog.h"

#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Common/UdpSocketBuilder.h"
#include "IPAddress.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDP_DataReceiver.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FUDP_DataReceiverDelegate, UPARAM(ref) FCamAndLight, CameraAndLight);


UCLASS()
class ARA_DATASETPLUGIN_API AUDP_DataReceiver : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUDP_DataReceiver();

	/* Returns the ID of the new connection. */
	UFUNCTION(BlueprintCallable, Category = "UDP Receiver | CamAndLignt struct")
		void Start_UDP_CL_Receiver
		(
			const FString& ipAddress, 
			int32 port,
			const FUDP_DataReceiverDelegate& OnCameraAndLightReceived
		);

	/* Disconnect from connection ID. */
	UFUNCTION(BlueprintCallable, Category = "UDP Receiver | CamAndLignt struct")
		void Stop_UDP_CL_Receiver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ExecuteOnMessageReceived(TWeakObjectPtr<AUDP_DataReceiver> thisObj);


	/* Used by the separate threads to print to console on the main thread. */
	static void PrintToConsole(FString Str, bool Error);

	int32 ReceiveBufferSize = sizeof(CamAndLight);

	/* Time between ticks. Please account for the fact that it takes 1ms to wake up on a modern PC, so 0.01f would effectively be 0.011f */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time between ticks")
		float TimeBetweenTicks = 0.0009f;

private:
	TUniquePtr<class FUDP_DataReceiverWorker> udp_data_receiver_worker;
	FUDP_DataReceiverDelegate data_received_delegate;

};

class FUDP_DataReceiverWorker : public FRunnable, public TSharedFromThis<FUDP_DataReceiverWorker>
{

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread = nullptr;

private:
	class FSocket* ConnectionSocket;
	FString ipAddress;
	int port;
	TWeakObjectPtr<AUDP_DataReceiver> ThreadSpawnerActor;
	int32 RecvBufferSize;
	int32 ActualRecvBufferSize;
	float TimeBetweenTicks;
	// SPSC = single producer, single consumer.
	TQueue<CamAndLight, EQueueMode::Spsc> Inbox; 

public:

	//Constructor / Destructor
	FUDP_DataReceiverWorker
	(
		FString inIp, 
		const int32 inPort, 
		TWeakObjectPtr<AUDP_DataReceiver> InOwner, 
		int32 inRecvBufferSize, 
		float inTimeBetweenTicks
	);
	
	virtual ~FUDP_DataReceiverWorker();

	/*  Starts processing of the connection. Needs to be called immediately after construction	 */
	void Start();

	/* Reads a message from the inbox queue */
	CamAndLight ReadFromInbox();

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
