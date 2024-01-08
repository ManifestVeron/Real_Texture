// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ImageContainer.h"

THIRD_PARTY_INCLUDES_START
#include "Header.pb.h"
#include "Iproto.h"

#include "opencv2/opencv.hpp"

THIRD_PARTY_INCLUDES_END

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Async/Async.h"
#include "Logging/MessageLog.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VF_FrameSender.generated.h"

// USTRUCT For image resizing if need

USTRUCT(BluePrintType)
struct FResizeImgParams
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NewWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NewHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int imQuality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNeedResize;

	FResizeImgParams():
		NewWidth(0),
		NewHeight(0),
		imQuality(0),
		bNeedResize(false)
	{}

	FResizeImgParams(int inWidth, int inHeight, int inQuality, bool inNeedResize) :
		NewWidth(inWidth),
		NewHeight(inHeight),
		imQuality(inQuality),
		bNeedResize(inNeedResize)
	{}

};

// Actor class for send Image Container
UCLASS()
class VFLIBPLUGIN_API AVF_FrameSender : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVF_FrameSender();

	// Start Server
	UFUNCTION(BlueprintCallable, Category = "VF Frame Sender")
		void Start_VF_Frame_Sender
		(
			FString ipAddress, 
			int32 port,
			int32 frameSize, 
			int32 sendDelay,
			FString ImgsSubfolder,
			bool bNeedSaveImgs,
			FString TelemetrySubfolder,
			bool bNeedSaveTelemetry
		);

	// Stop Server
	UFUNCTION(BlueprintCallable, Category = "VF Frame Sender")
		void Stop_VF_Frame_Sender();

	UFUNCTION(BlueprintCallable, Category = "VF Frame Sender")
		void SendImageContainer
		(
			FImageContainer img_container, 
			FResizeImgParams resize_params = FResizeImgParams()
		);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Used by the separate threads to print to console on the main thread. */
	static void PrintToConsole(FString Str, bool Error);

	/* Time between ticks. Please account for the fact that it takes 1ms to wake up on a modern PC, so 0.01f would effectively be 0.011f */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VF Frame Sender")
		float TimeBetweenTicksForSender = 0.0009f;

private:
	TUniquePtr<class FVF_FrameSenderWorker> frameSenderWorker;

};

// Worker class for send Image Container
class FVF_FrameSenderWorker : public FRunnable, public TSharedFromThis<FVF_FrameSenderWorker>
{
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread = nullptr;

private:
	
	// For Connection
	FString ipAddress;
	int32 port;
	int32 frameSize;
	int32 sendDelay;

	// For Logging
	FString ImgsSubfolder;
	bool bNeedSaveImgs;
	FString TelemetrySubfolder;
	bool bNeedSaveTelemetry;

	FResizeImgParams mResizeImgParams;

	// Fro Main Loop
	float TimeBetweenTicks;

	// For buffering
	TQueue<FImageContainer, EQueueMode::Spsc> OutBox;

	TWeakObjectPtr<AVF_FrameSender> ThreadSpawnerActor;

public:
	FVF_FrameSenderWorker
	(
		FString inIp,
		int32 inPort,
		int32 inframeSize,
		int32 insendDelay,
		FString inImgsSubfolder,
		bool inbNeedSaveImgs,
		FString inTelemetrySubfolder,
		bool inbNeedSaveTelemetry,
		TWeakObjectPtr<AVF_FrameSender> InOwner,
		float inTimeBetweenTicks
	);

	virtual ~FVF_FrameSenderWorker();

	/*  Starts processing of the connection. Needs to be called immediately after construction	 */
	void Start();

	void SetImgResizeParams(FResizeImgParams inParams);

	/* Adds a message to the outgoing message queue */
	void AddToOutbox(FImageContainer image_container);

	void SaveImage(FImageStruct image_struct, FString FileName);
	
	void SaveImage(cv::Mat mat_image, E_PR_ImageFormat ImgFormat, FString FileName);

	void SaveMetaData
	(
		FString FileName,
		FString SubFolder,
		int32 MetaIndex,
		FString MetaString
	);

	void ClearMetaFile(FString FileName, FString SubFolder);

	void ClearDirectory(FString SubFolder);

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	// End FRunnable interface	

	/** Shuts down the thread */
	void ConnectionShutdown();

private:

	/** thread should continue running */
	FThreadSafeBool bRun = false;

	FCriticalSection m_mutex;

};
