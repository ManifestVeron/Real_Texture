// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ARA_DOUBLE_STRUCT.h"

#include "CoreMinimal.h"
#include "ARA_InterestPoint.generated.h"

/**
 * 
 */
USTRUCT(BluePrintType)
struct FARA_InterestPoint
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT lat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT lon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT alt;

	FARA_InterestPoint()
	{
	}

	FARA_InterestPoint(double v_lat, double v_lon, double v_alt)
	{
		lat = FARA_DOUBLE_STRUCT(v_lat);
		lon = FARA_DOUBLE_STRUCT(v_lon);
		alt = FARA_DOUBLE_STRUCT(v_alt);
	}

};
