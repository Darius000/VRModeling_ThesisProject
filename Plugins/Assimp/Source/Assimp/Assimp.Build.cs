// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class Assimp : ModuleRules
{
    public Assimp(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
			}
            );


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
                "Projects",
                "Engine",
                "RenderCore",
                "ShaderCore",
                "RHI",
                "InputCore",
                "Interactions"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

        LoadAssimp(Target);

    }

    public bool LoadAssimp(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
        {
            isLibrarySupported = true;


            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "Win64" : "Win32";
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib\\" + PlatformString + "\\assimp-vc140-mt.lib"));
        }

        if (isLibrarySupported)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
        }

        PublicDefinitions.Add(string.Format("WITH_ASSIMP_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }  
}
