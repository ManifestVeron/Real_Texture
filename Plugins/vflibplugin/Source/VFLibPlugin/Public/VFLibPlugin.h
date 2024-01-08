// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FVFLibPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// VF Server Lib
	void* VFServer_Handle;

	// OpenCV Lib
	void* opencv_world_Handle;
	void* opencv_videoio_ffmpeg_Handle;

};
