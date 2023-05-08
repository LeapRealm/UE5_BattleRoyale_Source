// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleRoyal : ModuleRules
{
	public BattleRoyal(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", 
			"UMG", "Slate", "SlateCore", "Niagara", "MetasoundEngine", "GameplayCameras"
		});
		
		PrivateIncludePaths.AddRange(new string[]
		{
			"BattleRoyal",
			"BattleRoyal/Component",
			"BattleRoyal/Framework",
			"BattleRoyal/UI",
			"BattleRoyal/UI/Inventory",
			"BattleRoyal/UI/Navigation",
			"BattleRoyal/UI/Reticle",
			"BattleRoyal/UI/Status",
		});
	}
}
