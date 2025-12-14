// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FibulaServerTarget : TargetRules
{
	public FibulaServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("Fibula");
		bUseIris = true;
		MinCpuArchX64 = MinimumCpuArchitectureX64.AVX2;
		
		// Disable online subsystems we don't use
		GlobalDefinitions.Add("UE_DISABLE_ONLINE_SUBSYSTEM_EOS=1");
		GlobalDefinitions.Add("UE_DISABLE_ONLINE_SUBSYSTEM_STEAM=1");
	}
} 
