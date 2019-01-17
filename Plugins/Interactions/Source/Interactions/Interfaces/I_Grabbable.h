// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "I_Grabbable.generated.h"



// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_Grabbable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INTERACTIONS_API II_Grabbable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Grab(USceneComponent* attatchTo, FName socket, EControllerHand hand);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Release();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void SetOutline(bool inEnable, int32 inStencil);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void TriggerPressed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void TriggerReleased();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void CanBeGrabbed(bool& IsGrabbableReady);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void SetGrabbableEnabled(bool enabled);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGrabSigniture, USceneComponent*, attatchTo, FName, socket, EControllerHand, hand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReleaseSigniture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSetOutlineSigniture, bool, inEnable, int32, inStencil);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriggerPressedSigniture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriggerReleasedSigniture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGrabEnabledSigniture, bool, enabled);