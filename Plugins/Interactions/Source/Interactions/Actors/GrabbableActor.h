// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "Interactions/Interfaces/I_Grabbable.h"
#include "GrabbableActor.generated.h"



/**
 * 
 */
UCLASS(ClassGroup = (Interact))
class INTERACTIONS_API AGrabbableActor : public AInteractableActor, public II_Grabbable
{
	GENERATED_BODY()

public:

	AGrabbableActor();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool canGrab;

public:
	virtual void CanBeGrabbed_Implementation(bool& IsGrabbableReady) override;

	virtual void Grab_Implementation(USceneComponent* attatchTo, FName socket, EControllerHand hand) override;

	virtual void Release_Implementation() override;

	virtual void SetGrabbableEnabled_Implementation(bool enabled) override;

	virtual void SetOutline_Implementation(bool inEnable, int32 inStencil) override;

	virtual void TriggerPressed_Implementation() override;

	virtual void TriggerReleased_Implementation() override;
};
