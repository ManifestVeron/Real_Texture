// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConfigParameter.generated.h"


USTRUCT(BlueprintType)
struct FConfigParameter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FString Name;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
		FString Value;

	FConfigParameter()
	{
		Name = TEXT("");
		Value = TEXT("NONE");
	}

	FConfigParameter(FString vName, FString vValue)
	{
		Name = vName;
		Value = vValue;
	}
};

USTRUCT(BlueprintType)
struct FParametersList
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FConfigParameter> parameters;

	void Add(FConfigParameter vParameter);

	void Clear();

};
