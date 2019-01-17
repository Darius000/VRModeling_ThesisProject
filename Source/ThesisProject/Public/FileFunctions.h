// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileFunctions.generated.h"

/**
 * 
 */
UCLASS()
class THESISPROJECT_API UFileFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		static bool GetFiles(TArray<FString>& Files, TArray<FString>& FilePaths, FString Directory, FString Ext = "", bool bFiles = true, bool bDirectories = false, bool bRootDirectory = true);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static FString GetGameDirectory();
};
