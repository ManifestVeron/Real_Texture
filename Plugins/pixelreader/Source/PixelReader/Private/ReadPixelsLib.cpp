// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadPixelsLib.h"

//
bool ReadPixelsLib::ThreadSafe_ReadPixels
(	FRenderTarget* RT,
	TArray<FColor>& OutImageData,
	float Time,
	FReadSurfaceDataFlags InFlags,
	FIntRect InRect
)
{
	if (InRect == FIntRect(0, 0, 0, 0))
	{
		InRect = FIntRect(0, 0, RT->GetSizeXY().X, RT->GetSizeXY().Y);
	}

	// Read the render target surface data back.    
	struct FReadSurfaceContext
	{
		FRenderTarget* SrcRenderTarget;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};
	
	OutImageData.Reset();
	FReadSurfaceContext ReadSurfaceContext =
	{
		RT,
		&OutImageData,
		InRect,
		InFlags
	};


	FReadSurfaceContext Context = ReadSurfaceContext;
	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)
	(
			[Context](FRHICommandListImmediate& RHICmdList)
			{
				RHICmdList.ReadSurfaceData(
					Context.SrcRenderTarget->GetRenderTargetTexture(),
					Context.Rect,
					*Context.OutData,
					Context.Flags
				);
			}
	);

	while (!OutImageData.Num())
	{
		FPlatformProcess::Sleep(Time);
	}

	FPlatformProcess::Sleep(Time);

	return OutImageData.Num() > 0;
}

//
void ReadPixelsLib::ThreadSafe_ReadPixels
(
	FRenderTarget* RT,
	TArray<FColor>& OutImageData,
	FReadSurfaceDataFlags InFlags,
	FIntRect InRect
)
{
	if (InRect == FIntRect(0, 0, 0, 0))
	{
		InRect = FIntRect(0, 0, RT->GetSizeXY().X, RT->GetSizeXY().Y);
	}

	// Read the render target surface data back.    
	struct FReadSurfaceContext
	{
		FRenderTarget* SrcRenderTarget;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};
	
	OutImageData.Reset();
	FReadSurfaceContext ReadSurfaceContext =
	{
		RT,
		&OutImageData,
		InRect,
		InFlags
	};


	FReadSurfaceContext Context = ReadSurfaceContext;
	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)
	(
			[Context](FRHICommandListImmediate& RHICmdList)
			{
				RHICmdList.ReadSurfaceData(
					Context.SrcRenderTarget->GetRenderTargetTexture(),
					Context.Rect,
					*Context.OutData,
					Context.Flags
				);
			}
	);
}
