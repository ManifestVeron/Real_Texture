// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ARA_DOUBLE_STRUCT.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonOperations.generated.h"

/**
 * 
 */
UCLASS()
class ARA_NETWORKS_API UCommonOperations : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="ARA Networks. Data types")
	static float ARA_DOUBLE_STRUCT_As_Float(FARA_DOUBLE_STRUCT value);

};
