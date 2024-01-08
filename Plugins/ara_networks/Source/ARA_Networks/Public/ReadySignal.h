// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReadySignal.generated.h"

struct ReadySignal 
{
	int channel;
	int64 n_tick;

	ReadySignal()
	{
		channel	= 0;
		n_tick = 0;
	}

	ReadySignal(FReadySignal fready_signal);
};

USTRUCT(BluePrintType)
struct FReadySignal
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 channel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 n_tick;

	FReadySignal()
	{
		channel = 0;
		n_tick	= 0;
	}

	FReadySignal(ReadySignal ready_signal);
	ReadySignal GetRawStruct();
};
