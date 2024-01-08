// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VF_DOUBLE_STRUCT.h"
#include "ImageStruct.h"

#include "CoreMinimal.h"
#include "ImageContainer.generated.h"

// 
USTRUCT(BluePrintType)
struct FNavigation
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float gps_course;
	
	FNavigation()
	{
		gps_course = 0.0f;
	}
};

//
USTRUCT(BluePrintType)
struct FCameraSight
{
	GENERATED_BODY();
	// Параметры камеры
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float CaptureCamFov;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MainCamFov;
	// Ориентация камеры
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float yaw;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float roll;

	FCameraSight()
	{
		CaptureCamFov = MainCamFov = 0;
		yaw = pitch = roll = 0;
	}
};

// 
USTRUCT(BluePrintType)
struct FUAVData
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int32 boardNumber;
	
	// Позиция БЛА
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FVF_DOUBLE_STRUCT latitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FVF_DOUBLE_STRUCT longitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FVF_DOUBLE_STRUCT altitude;

	// Ориентация БЛА
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float yaw;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float roll;

	FUAVData()
	{
		boardNumber = 0;

		yaw = pitch = roll =0.0f;
	}
};

//
USTRUCT(BluePrintType)
struct FImageContainer
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int32 idx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FImageStruct image_struct;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Variables")
		FNavigation navigation;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Variables")
		FUAVData uav_data;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Variables")
		FCameraSight uav_camera_sight;

	FImageContainer():
		idx(0)
	{
	}
};
