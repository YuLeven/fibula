// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FibulaClientTarget : TargetRules
{
	public FibulaClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("Fibula");
		bUseIris = true;
		
		// Disable online subsystems we don't use
		GlobalDefinitions.Add("UE_DISABLE_ONLINE_SUBSYSTEM_EOS=1");
		GlobalDefinitions.Add("UE_DISABLE_ONLINE_SUBSYSTEM_STEAM=1");
	}
} 