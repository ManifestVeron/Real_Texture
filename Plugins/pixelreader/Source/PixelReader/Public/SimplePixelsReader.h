// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ImageStruct.h"
#include "ReadPixelsLib.h"

#include "Async/Async.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimplePixelsReader.generated.h"


UCLASS()
class PIXELREADER_API ASimplePixelsReader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimplePixelsReader();

	UPROPERTY(EditAnywhere, Category="RenderTarget")
	UTextureRenderTarget2D* RenderTarget;
	
	UFUNCTION(BlueprintCallable, Category="Simple Pixels Reader | Capture")
		void CaptureImage
		(
			FImageStructDelegate Out, 
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			int imQuality = 0
		);

	UFUNCTION(BlueprintCallable, Category = "Simple Pixels Reader | Capture")
		void CaptureImageAsRaw
		(
			FImageStructDelegate Out
		);

	UFUNCTION(BlueprintCallable, Category="Simple Pixels Reader | Capture and Save")
		void CaptureImageAndSave
		(
			FString FileName,
			FString SubFolder,
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			int imQuality = 0
		);
	
	UFUNCTION(BlueprintCallable, Category="Simple Pixels Reader | Capture with Save")
		void CaptureImageWithSave
		(
			FImageStructDelegate Out,
			FString FileName,
			FString SubFolder,
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			int imQuality = 0,
			bool bNeedSaved = true
		);

	UFUNCTION(BlueprintCallable, Category = "Simple Pixels Reader | Capture with Save")
		void CaptureImageAsRawWithSave
		(
			FImageStructDelegate RawOut,
			FString FileName,
			FString SubFolder,
			E_PR_ImageFormat imFormat = E_PR_ImageFormat::PNG,
			bool bNeedSaved = true
		);

	UFUNCTION(BlueprintCallable, Category = "Simple Pixels Reader | Render Target")
		void SetRenderTarget(UTextureRenderTarget2D* p_RenderTarget)
	{
		RenderTarget = p_RenderTarget;
	}

	UFUNCTION(BlueprintCallable, Category = "Simple Pixels Reader | Render Target")
	UTextureRenderTarget2D* GetRenderTarget()
	{
		return RenderTarget;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bReadPixelsStarted;
	FRenderCommandFence ReadPixelsFence;
	TArray<FColor> ColorBuffer;

	bool UpdateBuffer(FRenderTarget* RenderResource);

};
