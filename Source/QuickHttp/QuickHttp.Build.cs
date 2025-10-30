// Copyright 2025 NextLevelPlugins LLC. All Rights Reserved.

using UnrealBuildTool;
public class QuickHttp : ModuleRules
{
    public QuickHttp(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core","CoreUObject","Engine","HTTP","Json","JsonUtilities","Projects" });
    }
}
