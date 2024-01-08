// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarkupStruct.generated.h"

// Used types of Markup
UENUM(BluePrintType)
enum class E_PR_MarkupType : uint8
{
	KITTY UMETA(DisplayName = "KITTY"),
	JSON UMETA(DisplayName = "JSON")
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FMarkup
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 objectId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 objectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FVector2D ScreenPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FVector2D ObjectScreenSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		bool isRelativeCoordinates;

	FMarkup()
	{
		id = 0;
		objectId = 0;
		objectClass = 0;
		distance = 0;
		ScreenPosition = FVector2D(0, 0);
		ObjectScreenSize = FVector2D(0, 0);
		isRelativeCoordinates = false;
	}
};

USTRUCT(BlueprintType)
struct FMarkupMeta
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 trajectory_index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CanDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool TrajectoryStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 WeatherCode;

};

USTRUCT(BlueprintType)
struct FMarkupStruct
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		E_PR_MarkupType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMarkupMeta markup_meta;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> markup_bytes;

};
