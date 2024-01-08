// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARA_DatasetPlugin.h"

#define LOCTEXT_NAMESPACE "FARA_DatasetPluginModule"

void FARA_DatasetPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FARA_DatasetPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FARA_DatasetPluginModule, ARA_DatasetPlugin)