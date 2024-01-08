// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ARA_DOUBLE_STRUCT.h"

#include "CoreMinimal.h"
#include "EnvPacket.generated.h"

#pragma pack(push, 1)

// Струкура описания цели
struct Target
{
	char var_id;
	char placement_id;
	char class_id;
	double latitude;
	double longitude;
	double altitude;
	double pitch;
	double roll;
	double course;
};

// Структура описания пакета, принимаемого от имитатора окружения

#define TARGETS_COUNT 100

struct EnvPacket
{
	Target targets[TARGETS_COUNT];
};

#pragma pack(pop)


// Структура для представления цели в BP

USTRUCT(BluePrintType)
struct FTarget
{
	GENERATED_BODY();

	// Target meta data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 var_id;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 placement_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 class_id;

	// Position
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT latitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT longitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT altitude;

	// Orientation

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT roll;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FARA_DOUBLE_STRUCT course;
	
	FTarget()
	{
		var_id = 0;
		placement_id = 0;
		class_id = 0;
	}

	FTarget(Target v_target)
	{
		var_id 				= (unsigned char)v_target.var_id;
		placement_id 		= (unsigned char)v_target.placement_id;
		class_id 			= (unsigned char)v_target.class_id;
		
		latitude.make_doubleStruct(v_target.latitude);
		longitude.make_doubleStruct(v_target.longitude);
		altitude.make_doubleStruct(v_target.altitude);

		pitch.make_doubleStruct(v_target.pitch);
		roll.make_doubleStruct(v_target.roll);
		course.make_doubleStruct(v_target.course);

	}
};
