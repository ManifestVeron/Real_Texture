// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjLibPlugin.h"
#include "Engine.h"
#include "Interfaces/IPluginManager.h"
#include "Containers/UnrealString.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FProjLibPluginModule"

void FProjLibPluginModule::StartupModule()
{
	const FString BasePluginDir = IPluginManager::Get().FindPlugin("ProjLibPlugin")->GetBaseDir();
	

	// lzma
	const FString lzma_Path = FPaths::Combine(*BasePluginDir, TEXT("Source/proj-8.1.1/x64/bin/lzma.dll"));
	lzma_Handle = FPlatformProcess::GetDllHandle(*lzma_Path);
	
	if (lzma_Handle != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("lzma.dll loaded successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("lzma.dll failed to load!"));
	}
	
	// sqlite3
	const FString sqlite3_Path = FPaths::Combine(*BasePluginDir, TEXT("Source/proj-8.1.1/x64/bin/sqlite3.dll"));
	sqlite3_Handle = FPlatformProcess::GetDllHandle(*sqlite3_Path);
	
	if (sqlite3_Handle != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("sqlite3.dll loaded successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("sqlite3.dll failed to load!"));
	}
	
}

void FProjLibPluginModule::ShutdownModule()
{
	// libcurl
	FPlatformProcess::FreeDllHandle(libcurl_Handle);
	libcurl_Handle = nullptr;
	
	// lzma
	FPlatformProcess::FreeDllHandle(lzma_Handle);
	lzma_Handle = nullptr;
	
	// sqlite3
	FPlatformProcess::FreeDllHandle(sqlite3_Handle);
	sqlite3_Handle = nullptr;
	
	// tiff
	FPlatformProcess::FreeDllHandle(tiff_Handle);
	tiff_Handle = nullptr;
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProjLibPluginModule, ProjLibPlugin)