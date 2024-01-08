// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARA_ConfigsPlugin.h"

#define LOCTEXT_NAMESPACE "FARA_ConfigsPluginModule"

void FARA_ConfigsPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FARA_ConfigsPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FARA_ConfigsPluginModule, ARA_ConfigsPlugin)