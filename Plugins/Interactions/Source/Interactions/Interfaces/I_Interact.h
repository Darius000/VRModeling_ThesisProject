// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_Interact.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_Interact : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INTERACTIONS_API II_Interact
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void InteractWith(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool WasInteractedWith();
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractSigniture, AActor*, actor);