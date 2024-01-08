// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ImageStruct.h"

#include "Async/Async.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "ReadPixelsLib.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AsyncCaptureLibrary.generated.h"


//
UCLASS()
class PIXELREADER_API UAsyncCaptureLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Асинхронный захват по длительности. С делегатом
	UFUNCTION(BluePrintCallable, Category = "Async Capture Library | Capture. Image Delegated")
		static void AsyncCaptureImageDelegated
		(
			FImageStructDelegate Out,
			USceneCaptureComponent2D* CameraCapture,
			float Time,
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			int imQuality=0
		);

	// Асинхронный захват красного канала по длительности. С делегатом
	UFUNCTION(BluePrintCallable, Category = "Async Capture Library | Capture raw. Image Delegated")
		static void AsyncCaptureImageRawDelegated
		(
			FImageStructDelegate Out,
			USceneCaptureComponent2D* CameraCapture,
			float Time
		);

	// Асинхронный захват под длительности. С сохранением. С делегатом
	UFUNCTION(BluePrintCallable, Category = "Async Capture Library | Capture with Save. Image Delegated")
		static void AsyncCaptureImageWithSaveDelegated
		(
			FImageStructDelegate Out,
			USceneCaptureComponent2D* CameraCapture,
			float Time,
			FString FileName,
			FString SubFolder,
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			int imQuality = 0,
			bool bNeedSaved = true
		);

	// Асинхронный захват по ловушке. С сохранением и возвратом
	UFUNCTION(BluePrintCallable, Category = "Async Capture Library | Capture with Save. Reterned by Fence")
		static bool AsyncCaptureImageWithSaveReternedByFence
		(
			FImageStruct& img_data,
			USceneCaptureComponent2D* CameraCapture,
			FString FileName, 
			FString SubFolder, 
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			int imQuality = 0,
			bool bNeedSaved = true
		);

	// Асинхронный захват красного канала по ловушке. С сохранением и возвратом
	UFUNCTION(BluePrintCallable, Category = "Async Capture Library | Capture raw. Reterned by Fence")
		static bool AsyncCaptureImageRawReternedByFence
		(
			FImageStruct& img_data,
			USceneCaptureComponent2D* CameraCapture
		);

	// Асинхронный захват под длительности. С сохранением и возвратом
	UFUNCTION(BluePrintCallable, Category = "Async Capture Library | Capture with Save. Reterned by Time")
		static bool AsyncCaptureImageWithSaveReterned
		(
			FImageStruct& img_data,
			USceneCaptureComponent2D* CameraCapture,
			float Time,
			FString FileName,
			FString SubFolder,
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			int imQuality = 0,
			bool bNeedSaved = true
		);
};
