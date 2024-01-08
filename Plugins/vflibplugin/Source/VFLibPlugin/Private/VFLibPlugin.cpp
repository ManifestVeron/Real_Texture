// Copyright Epic Games, Inc. All Rights Reserved.

#include "VFLibPlugin.h"

#include "Engine.h"
#include "Interfaces/IPluginManager.h"
#include "Containers/UnrealString.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FVFLibPluginModule"

void FVFLibPluginModule::StartupModule()
{
	const FString BasePluginDir = IPluginManager::Get().FindPlugin("VFLibPlugin")->GetBaseDir();

	// VFServer
	const FString VFServer_Path = FPaths::Combine(*BasePluginDir, TEXT("Source/VFProto/x64/bin/VFServer.dll"));
	VFServer_Handle = FPlatformProcess::GetDllHandle(*VFServer_Path);
	
	if (VFServer_Handle != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("VFServer.dll loaded successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("VFServer.dll failed to load!"));
	}

	// OpenCV
	const FString opencv_world_Path = FPaths::Combine(*BasePluginDir, TEXT("Source/OpenCV/Libraries/Win64/opencv_world451.dll"));
	opencv_world_Handle = FPlatformProcess::GetDllHandle(*opencv_world_Path);
	
	const FString opencv_videoio_ffmpeg_Path = FPaths::Combine(*BasePluginDir, TEXT("Source/OpenCV/Libraries/Win64/opencv_videoio_ffmpeg451_64.dll"));
	opencv_videoio_ffmpeg_Handle = FPlatformProcess::GetDllHandle(*opencv_videoio_ffmpeg_Path);

	if (opencv_world_Handle != nullptr ||
		opencv_videoio_ffmpeg_Handle != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("OpenCV loaded successfully!"));
	}
	else 
	{
		UE_LOG(LogTemp, Fatal, TEXT("Some OpenCV DLL failed to load!"));
	}
}

void FVFLibPluginModule::ShutdownModule()
{
	// VFServer
	FPlatformProcess::FreeDllHandle(VFServer_Handle);
	VFServer_Handle = nullptr;

	// OpenCV
	
	FPlatformProcess::FreeDllHandle(opencv_world_Handle);
	opencv_world_Handle = nullptr;

	FPlatformProcess::FreeDllHandle(opencv_videoio_ffmpeg_Handle);
	opencv_videoio_ffmpeg_Handle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVFLibPluginModule, VFLibPlugin)