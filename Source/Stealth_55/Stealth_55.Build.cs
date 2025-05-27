// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Stealth_55 : ModuleRules
{
	public Stealth_55(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
