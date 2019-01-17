// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Assimp.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FAssimpModule"

void FAssimpModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("Assimp")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/Assimp/Lib/Win64/assimp-vc140-mt.dll"));
#endif // PLATFORM_WINDOWS
	
	AssimpHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	/*UE_LOG(LogTemp, Warning, TEXT("%s"), *BaseDir);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *LibraryPath);*/

	if (AssimpHandle)
	{
		// Call the test function in the third party library that opens a message box
		//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Assimp Loaded"));

		UE_LOG(LogTemp, Log, TEXT("Assimp Loaded"));
	}
	else
	{
		//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load Assimp"));

		UE_LOG(LogTemp, Log, TEXT("Unable to Load Assimp"));
	}
}

void FAssimpModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(AssimpHandle);
	AssimpHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssimpModule, Assimp)
