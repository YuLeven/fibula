// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Fibula : ModuleRules
{
	public Fibula(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "SlateCore", "Niagara", "Json", "JsonUtilities", "HTTP", "RHI", "Slate" });
		SetupIrisSupport(Target);
	}
}
