// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RHI.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"

#include "CoreMinimal.h"

/**
 * 
 */
class PIXELREADER_API ReadPixelsLib
{
public:

/**
 * @brief 
 * 
 * @param RT Render Target as pointer
 * @param OutImageData  Image buffers as TArray<FColor>
 * @param Time float in seconds
 * @param InFlags 
 * @param InRect 
 * @return true or false
 */
	static bool ThreadSafe_ReadPixels
	(
		FRenderTarget* RT,
		TArray<FColor>& OutImageData,
		float Time,
		FReadSurfaceDataFlags InFlags = FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX),
		FIntRect InRect = FIntRect(0, 0, 0, 0)	
	);

/**
 * @brief 
 * 
 * @param RT Render Target as pointer
 * @param OutImageData Image buffers as TArray<FColor>
 * @param InFlags 
 * @param InRect 
 */
	static void ThreadSafe_ReadPixels
	(
		FRenderTarget* RT,
		TArray<FColor>& OutImageData,
		FReadSurfaceDataFlags InFlags = FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX),
		FIntRect InRect = FIntRect(0,0,0,0)
	);

private:
	ReadPixelsLib(){};
	~ReadPixelsLib(){};
	ReadPixelsLib(const ReadPixelsLib&) = delete;
	ReadPixelsLib& operator= (const ReadPixelsLib&) = delete;
};
