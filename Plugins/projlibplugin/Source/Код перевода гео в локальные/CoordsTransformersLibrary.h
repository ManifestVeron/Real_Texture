// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GEO_DOUBLE_STRUCT.h"

#include "CoordsTransformer.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoordsTransformersLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJLIBPLUGIN_API UCoordsTransformersLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// Translate coordinates from Geo to UE
	UFUNCTION(BluePrintCallable, Category = "Coords Transformers")
		static void GEO_UE_Transformer
		(
			// Input params
			FString src_crs,
			FString dst_crs, 

			FGEO_DOUBLE_STRUCT lat0, 
			FGEO_DOUBLE_STRUCT lon0,
			FGEO_DOUBLE_STRUCT lat, 
			FGEO_DOUBLE_STRUCT lon,
			FGEO_DOUBLE_STRUCT alt,
			FGEO_DOUBLE_STRUCT z_fix,

			// Output params
			float& x,
			float& y,
			float& z
		);

	// Translate coordinates from UE to Geo
	UFUNCTION(BluePrintCallable, Category = "Coords Transformers")
		static void UE_GEO_Transformer
		(
			// Input params
			FString src_crs,
			FString dst_crs,

			FGEO_DOUBLE_STRUCT lat0,
			FGEO_DOUBLE_STRUCT lon0,
			float x,
			float y,
			float z,
			FGEO_DOUBLE_STRUCT z_fix,
			// Output params
			FGEO_DOUBLE_STRUCT& lat,
			FGEO_DOUBLE_STRUCT& lon,
			FGEO_DOUBLE_STRUCT& alt

		);

};
