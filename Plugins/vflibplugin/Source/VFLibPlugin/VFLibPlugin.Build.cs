// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using System;

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class VFLibPlugin : ModuleRules
{
	public VFLibPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
        /*
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrecompileForTargets = PrecompileTargetsType.Any;
		bPrecompile = true;
		*/
        //bUsePrecompiled = true;
        bEnableUndefinedIdentifierWarnings = false;

		// Include header files

		// VF Lib
		string VFLibPath = Path.Combine(ModuleDirectory, "..", "VFProto");

        PublicIncludePaths.Add(Path.Combine(VFLibPath, "x64", "include"));
		PublicIncludePaths.Add(Path.Combine(VFLibPath, "protobuf", "include"));
		PublicIncludePaths.Add(Path.Combine(VFLibPath, "protobuf", "protobuf-3.15.0", "src"));

		// OpenCV Lib
		string OpenCVPath = Path.Combine(ModuleDirectory, "..", "OpenCV");

		PublicIncludePaths.AddRange(new[] { Path.Combine(OpenCVPath, "includes") });

        PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Projects",
                "PixelReader",
                "RHI",
                "RenderCore",
                "Media",
                "MediaAssets"
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

		if(Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Include import libraries
			
			// VFServer Lib
			PublicAdditionalLibraries.Add(Path.Combine(VFLibPath, "x64", "lib", "VFServer.lib"));

			// OpenCV Lib
			PublicAdditionalLibraries.Add(Path.Combine(OpenCVPath, "Libraries", "Win64", "opencv_world451.lib"));

            // Put libraries along with executable
            
			// VFServer DLL
			RuntimeDependencies.Add("$(PluginDir)/Source/VFProto/x64/bin/VFServer.dll");

            //OpenCV DLLs
            RuntimeDependencies.Add("$(PluginDir)/Source/OpenCV/Libraries/Win64/opencv_world451.dll");
            RuntimeDependencies.Add("$(PluginDir)/Source/OpenCV/Libraries/Win64/opencv_videoio_ffmpeg451_64.dll");

            // Load dll libraries

            // VFServer DLL
            PublicDelayLoadDLLs.Add("VFServer.dll");
            
			// OpenCV DLLs
			PublicDelayLoadDLLs.Add("opencv_world451.dll");
            PublicDelayLoadDLLs.Add("opencv_videoio_ffmpeg451_64.dll");
        }
	}
}
