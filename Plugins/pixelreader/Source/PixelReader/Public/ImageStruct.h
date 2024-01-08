// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ImageStruct.generated.h"

// Used types of Image Format
UENUM(BluePrintType)
enum class E_PR_ImageFormat : uint8
{
	PNG UMETA(DisplayName = "PNG"),
	JPEG UMETA(DisplayName = "JPEG"),
	RAW_RED UMETA(DisplayName = "RAW_RED")
};

//	Image representation in blueprints
USTRUCT(BluePrintType)
struct FImageStruct
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> img_array;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 width;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 height;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		E_PR_ImageFormat img_format;

	FImageStruct()
	{

	}

	FImageStruct
	(
		TArray<uint8> v_img_array, 
		int32 v_width,
		int32 v_height, 
		E_PR_ImageFormat v_img_format
	)
	{
		img_array = v_img_array;
		width = v_width;
		height = v_height;
		img_format = v_img_format;
	}
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FImageStructDelegate, FImageStruct, img_struct);
