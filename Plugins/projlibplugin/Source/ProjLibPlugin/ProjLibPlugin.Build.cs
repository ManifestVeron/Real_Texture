// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using System;

using UnrealBuildTool;

public class ProjLibPlugin : ModuleRules
{
	public ProjLibPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		/*
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrecompileForTargets = PrecompileTargetsType.Any;
		bPrecompile = true;
		*/
		//bUsePrecompiled = true;

		// Include header files
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "..", "proj-8.1.1", "x64", "include"));

		PrivateIncludePaths.AddRange(
			new string[] {
				//
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Projects"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Include static library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "..", "proj-8.1.1", "x64", "lib", "proj.lib"));

			// Include import libraries
			if(Target.Type == TargetType.Editor) 
				PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "..", "proj-8.1.1", "x64", "lib", "libcurl.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "..", "proj-8.1.1", "x64", "lib", "lzma.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "..", "proj-8.1.1", "x64", "lib", "sqlite3.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "..", "proj-8.1.1", "x64", "lib", "tiff.lib"));

			// Put libraries along with executable
			if (Target.Type == TargetType.Editor)
				RuntimeDependencies.Add("$(PluginDir)/Source/proj-8.1.1/x64/bin/libcurl.dll");
			RuntimeDependencies.Add("$(PluginDir)/Source/proj-8.1.1/x64/bin/lzma.dll");
			RuntimeDependencies.Add("$(PluginDir)/Source/proj-8.1.1/x64/bin/sqlite3.dll");
			RuntimeDependencies.Add("$(PluginDir)/Source/proj-8.1.1/x64/bin/tiff.dll");

			// Load dll libraries
			if (Target.Type == TargetType.Editor)
				PublicDelayLoadDLLs.Add("libcurl.dll");
			PublicDelayLoadDLLs.Add("lzma.dll");
			PublicDelayLoadDLLs.Add("sqlite3.dll");
			PublicDelayLoadDLLs.Add("tiff.dll");
		}

	}
}
