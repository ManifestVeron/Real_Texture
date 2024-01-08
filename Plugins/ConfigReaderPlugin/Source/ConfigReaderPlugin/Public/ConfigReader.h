// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "misc/FileHelper.h"
#include "misc/Paths.h"

#include "ConfigParameter.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ConfigReader.generated.h"

/**
 * 
 */
UCLASS()
class CONFIGREADERPLUGIN_API UConfigReader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// key=value
	UFUNCTION(BluePrintCallable, Category = "Config Reader | Simple Config File")
	static bool SimpleConfigLoader(TMap<FString, FString>& ParametersMap, FString configPath = "simpleConfig.cfg");

	// [sections]
	// key=value
	UFUNCTION(BluePrintCallable, Category = "Config Reader | Config File with Sections")
	static bool ConfigLoader(TMap<FString, FParametersList>& ParametersMap, FString configPath = "config.cfg");
};
