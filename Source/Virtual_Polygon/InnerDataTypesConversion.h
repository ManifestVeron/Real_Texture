// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ARA_DOUBLE_STRUCT.h"
#include "GEO_DOUBLE_STRUCT.h"
#include "VF_DOUBLE_STRUCT.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InnerDataTypesConversion.generated.h"

/**
 * 
 */
UCLASS()
class VIRTUAL_POLYGON_API UInnerDataTypesConversion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category="ARA. Data types convertions")
	static FGEO_DOUBLE_STRUCT INNER_CONVERT_ARA_GEO(FARA_DOUBLE_STRUCT value);

	UFUNCTION(BlueprintCallable, Category="ARA. Data types convertions")
	static FVF_DOUBLE_STRUCT INNER_CONVERT_ARA_VF(FARA_DOUBLE_STRUCT value);

	UFUNCTION(BlueprintCallable, Category="ARA. Data types convertions")
	static FARA_DOUBLE_STRUCT INNER_CONVERT_GEO_ARA(FGEO_DOUBLE_STRUCT value);

	UFUNCTION(BlueprintCallable, Category="ARA. Data types convertions")
	static FARA_DOUBLE_STRUCT INNER_CONVERT_VF_ARA(FVF_DOUBLE_STRUCT value);

	UFUNCTION(BlueprintCallable, Category="ARA. Data types convertions")
	static FGEO_DOUBLE_STRUCT INNER_CONVERT_VF_GEO(FVF_DOUBLE_STRUCT value);

	UFUNCTION(BlueprintCallable, Category="ARA. Data types convertions")
	static FVF_DOUBLE_STRUCT INNER_CONVERT_GEO_VF(FGEO_DOUBLE_STRUCT value);

};
