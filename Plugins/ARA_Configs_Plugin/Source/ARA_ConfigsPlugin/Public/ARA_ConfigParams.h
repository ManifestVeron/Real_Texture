// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARA_ConfigParams.generated.h"

// Типовая структура с параметрами соединения
USTRUCT(BlueprintType)
struct FARAConnectionParams
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 port;

	FARAConnectionParams()
	{
		ip = "127.0.0.1";
		port = 0;
	}

	FARAConnectionParams(FString v_ip, int32 v_port)
	{
		ip = v_ip;
		port = v_port;
	}

};

// Типовая структура с параметрами соединения
USTRUCT(BlueprintType)
struct FARA_VFSenderParams
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 port;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 frameSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 sendDelay;

	FARA_VFSenderParams():
		ip(TEXT("127.0.0.1")),
		port(1111),
		frameSize(32000),
		sendDelay(3)
	{
	}

	FARA_VFSenderParams
	(
		FString v_ip, 
		int32 v_port,
		int32 v_frameSize,
		int32 v_sendDelay
	):
		ip(v_ip),
		port(v_port),
		frameSize(v_frameSize),
		sendDelay(v_sendDelay)
	{
	}

};

// Типовая структура с параметрами захвата
USTRUCT(BlueprintType)
struct FARAImageCaptureParams
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bNeedSave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ImgFormat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ImgQuality;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ImgCustomDurationCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bActivateCustomDuration;

	FARAImageCaptureParams() :
		bNeedSave(true),
		ImgFormat(TEXT("jpg")),
		ImgQuality(0),
		ImgCustomDurationCapture(0.0f),
		bActivateCustomDuration(false)
	{
	}

	FARAImageCaptureParams
	(
		bool vNeedSave, 
		FString vImgFormat, 
		int32 vImgQuality=0,
		float vImgCustomDuration=0.0f,
		bool vActivateCustiomDuration=false
	):
		bNeedSave(vNeedSave),
		ImgFormat(vImgFormat),
		ImgQuality(vImgQuality),
		ImgCustomDurationCapture(vImgCustomDuration),
		bActivateCustomDuration(vActivateCustiomDuration)
	{
	}
};

// Типовая структура с параметрами настройки сцены
USTRUCT(BlueprintType)
struct FARASceneRenderingParams
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 GrassType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 ForestType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 StonesType;

	FARASceneRenderingParams()
	{
		GrassType = ForestType = StonesType = 0;
	}

	FARASceneRenderingParams
	(
		uint8 vGrassType,
		uint8 vForestType,
		uint8 vStonesType
	)
	{
		GrassType	= vGrassType;
		ForestType	= vForestType;
		StonesType	= vStonesType;
	}
};

// Структура с параметрами кастомизации RenderTarget
USTRUCT(BlueprintType)
struct FARAResizeParams
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Width;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Height;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ActivateParams;

	FARAResizeParams()
	{
		Width = Height = 0;
		ActivateParams = false;
	}

	FARAResizeParams(int width, int height, bool bActivate)
	{
		Width = width;
		Height = height;
		ActivateParams = bActivate;

		if (Width * Height == 0)
		{
			Width = Height = 0;
			ActivateParams = false;
		}
	}
};

// Экспериментальная структура с доп. параметрами
USTRUCT(BlueprintType)
struct FARADopParams
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString BoardNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUseIPointPeeper;

	FARADopParams()
	{
		BoardNumber			= "None";
		bUseIPointPeeper	= false;
	}

	FARADopParams(FString vBoardNumber, bool vUseIpointPeeper)
	{
		BoardNumber = vBoardNumber;
		bUseIPointPeeper = vUseIpointPeeper;
	}
};

// Типовая структура с параметрами построения интерактивной разметки
USTRUCT(BlueprintType)
struct FARAInteractiveMarkupParams
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bNeedSave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MrkpFormat;

	FARAInteractiveMarkupParams():
		bNeedSave(false),
		MrkpFormat(TEXT("JSON"))
	{
	}

	FARAInteractiveMarkupParams(bool vNeedSave, FString vMrkpFormat) :
		bNeedSave(vNeedSave),
		MrkpFormat(vMrkpFormat)
	{}
};

