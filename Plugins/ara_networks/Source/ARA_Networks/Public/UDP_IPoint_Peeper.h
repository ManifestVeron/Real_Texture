// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "nlohmann/json.hpp"

#include "ARA_InterestPoint.h"

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
#include "UDP_IPoint_Peeper.generated.h"

/**
 *  Actor Class. UDP Interest Point Peeper for Ineterest Point Altitude rectification
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FARA_RecvInterestPointDelegate, UPARAM(ref) FARA_InterestPoint, ipoint);

UCLASS()
class ARA_NETWORKS_API AUDP_IPoint_Peeper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUDP_IPoint_Peeper();

	UFUNCTION(BlueprintCallable, Category = "ARA Networks | UDP Interest Point Peeper")
		void Start_UDP_IPoint_Peeper
		(
			const FString& ipAddress, 
			int32 port,
			const FARA_RecvInterestPointDelegate& OnInterestedPointReceivedDelegate
		);

	UFUNCTION(BlueprintCallable, Category = "ARA Networks | UDP Interest Point Peeper")
		void Stop_UDP_IPoint_Peeper();
	
	UFUNCTION(BlueprintCallable, Category = "ARA Networks | UDP Interest Point Peeper")
		void SendIPointData(FARA_InterestPoint v_ipoint);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ExecuteOnInterestPointReceived(TWeakObjectPtr<AUDP_IPoint_Peeper> thisObj);

	static void PrintToConsole(FString Str, bool Error);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARA Networks | UDP Interest Point Peeper")
		float TimeBetweenTicks = 0.0009f;

private:
	TUniquePtr<class FUDP_IPointPeeperWorker> IPointPeeperWorker;
	FARA_RecvInterestPointDelegate RecvInterestPointDelegate;
};

//
// Worker Class.
//
class FUDP_IPointPeeperWorker : public FRunnable, public TSharedFromThis<AUDP_IPoint_Peeper>
{
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread = nullptr;

private:
	FSocket* RecvConnectionSocket;
	FString ipAddress;
	int port;
	TWeakObjectPtr<AUDP_IPoint_Peeper> ThreadSpawnerActor;
	float TimeBetweenTicks;

	TQueue<FARA_InterestPoint, EQueueMode::Spsc> Inbox;
	TQueue<FARA_InterestPoint, EQueueMode::Spsc> OutBox;
	
	FARA_InterestPoint control_ipoint;
	nlohmann::json control_json;

public:

	FUDP_IPointPeeperWorker
	(
		FString inIp,
		const int32 inPort,
		TWeakObjectPtr<AUDP_IPoint_Peeper> InOwner,
		float inTimeBetweenTicks
	);

	virtual ~FUDP_IPointPeeperWorker();

	/*  Starts processing of the connection. Needs to be called immediately after construction	 */
	void Start();

	FARA_InterestPoint ReadFromInbox();
	void AddToOutbox(FARA_InterestPoint v_ipoint);

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
