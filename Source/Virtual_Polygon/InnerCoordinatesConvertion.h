// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonOperations.h"
#include "MathPacket.h"
#include "EnvPacket.h"
#include "CoordsTransformersLibrary.h"
#include "InnerDataTypesConversion.h"
#include "Kismet/KismetMathLibrary.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InnerCoordinatesConvertion.generated.h"

/**
 * 
 */
UCLASS()
class VIRTUAL_POLYGON_API UInnerCoordinatesConvertion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ARA. Coordinates convertions")
	static void Orlan_GEO_UE
	(
		FString src_crs,
		FString dst_crs,
		FString lat0,
		FString lon0,
		FString z_fix, 
		FMathPacket math_packet,
		float& X,
		float& Y,
		float& Z,
		float& YAW, 
		float& PITCH,
		float& ROLL 
	);

	UFUNCTION(BlueprintCallable, Category="ARA. Coordinates convertions")
	static void Target_GEO_UE
	(
		FString src_crs,
		FString dst_crs,
		FString lat0,
		FString lon0,
		FString z_fix,
		FTarget target_packet,
		uint8& class_id,
		float& X,
		float& Y,
		float& Z,
		float& YAW, 
		float& PITCH,
		float& ROLL 
	);

};
