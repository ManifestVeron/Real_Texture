// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Virtual_Polygon : ModuleRules
{
	public Virtual_Polygon(ReadOnlyTargetRules Target) : base(Target)
	{
		/*
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrecompileForTargets = PrecompileTargetsType.Any;
		bPrecompile = true;
		*/
		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"ARA_Networks",
			"PixelReader",
			"ProjLibPlugin",
			"VFLibPlugin",
			"ARA_DatasetPlugin",
			"MarkupPlugin"
			//"EditorStyle"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
