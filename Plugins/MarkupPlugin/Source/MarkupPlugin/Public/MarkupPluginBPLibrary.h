// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraTypes.h"
#include "Engine/EngineTypes.h"
#include "Containers/Map.h"
#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"

#include "MarkupStruct.h"

#include "MarkupPluginBPLibrary.generated.h"


class AModels_Group;


UCLASS()
class UMarkupPluginBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
		

	UFUNCTION(BlueprintCallable, Category = "MarkupPlugin")
	static TArray<FMarkup> MakeMarkup
	(
		USceneCaptureComponent2D* Camera,
		TArray<AActor*> actors,
		TMap<UObject*, FVector> map,
		float hitThreshold = 0.5f,
		ECollisionChannel collisionChannel = ECC_Camera,
		EDrawDebugTrace::Type drawDebugType = EDrawDebugTrace::None,
		bool bTraceComplex = false,
		bool relativeCoordinates = true,
		bool center = true
		);

	UFUNCTION(BlueprintCallable, Category = "MarkupPlugin")
	static void GenerateJsonFile(TArray<FMarkup> array, FString nameFile);

	UFUNCTION(BlueprintCallable, Category = "MarkupPlugin")
	static FMarkupStruct GenerateJsonStrings(TArray<FMarkup> array);

	UFUNCTION(BlueprintCallable, Category = "MarkupPlugin")
	static void GenerateKittyFile
	(
		TArray<FMarkup> array,
		FString nameFile
	);

	UFUNCTION(BlueprintCallable, Category = "MarkupPlugin")
	static FMarkupStruct GenerateKittyStrings
	(
		TArray<FMarkup> array,
		int32 trajectory_index,
		float CanDistance = 0.0f,
		bool TrajectoryStart = false,
		int32 WeatherCode = 0
	);

	static bool EqualUnique_object_id(uint8 one , uint8 two)
	{
		return one == two;
	}

	static bool GenerateObjectMarkup
	(
		USceneCaptureComponent2D* Camera,
		AActor* Object,
		int Type,
		FMarkup& objectMarkup,
		bool relativeCoordinates,
		ETraceTypeQuery channelType,
		EDrawDebugTrace::Type drawDebugType,
		bool bTraceComplex,
		float hitThreshold
	);

	static bool VisibilityCheckMultitrace
	(
		USceneCaptureComponent2D* Camera,
		AActor* Object,
		ETraceTypeQuery channelType,
		EDrawDebugTrace::Type drawDebugType,
		bool bTraceComplex,
		float hitThreshold
	);

	static void GetObjectAxisAlignBoxQuad
	(
		USceneCaptureComponent2D* Camera,
		AActor* Object,
		FVector2D& ScreenPosition,
		FVector2D& ObjectScreenSize
	);

	static void RelativeRotation(AActor* Object, TArray<FVector4>& array);

	static void ProjectBoundingBoxCorners
	(
		USceneCaptureComponent2D* Camera,
		TArray<FVector4>& array,
		TArray<FVector2D>& vectors
	);

	static FVector2D ProjectWorldToScreen(USceneCaptureComponent2D* Camera, FVector4 vector);


};

