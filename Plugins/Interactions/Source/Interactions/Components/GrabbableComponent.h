// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.h"
#include "Interactions/Interfaces/I_Grabbable.h"
#include "GrabbableComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interact), meta = (BlueprintSpawnableComponent))
class INTERACTIONS_API UGrabbableComponent : public UInteractableComponent, public II_Grabbable
{
	GENERATED_BODY()

public:

	UGrabbableComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		bool IsGrabbed();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool canGrab;


protected:
	bool grabbed;

public:
	virtual void CanBeGrabbed_Implementation(bool& IsGrabbableReady) override;

	virtual void Grab_Implementation(USceneComponent* attatchTo, FName socket, EControllerHand hand) override;

	virtual void Release_Implementation() override;

	virtual void SetGrabbableEnabled_Implementation(bool enabled) override;

	virtual void SetOutline_Implementation(bool inEnable, int32 inStencil) override;

	virtual void TriggerPressed_Implementation() override;

	virtual void TriggerReleased_Implementation() override;

	UPROPERTY(BlueprintAssignable)
		FGrabSigniture OnGrab;

	UPROPERTY(BlueprintAssignable)
		FReleaseSigniture OnRelease;

	UPROPERTY(BlueprintAssignable)
		FSetOutlineSigniture OnSetOutline;

	UPROPERTY(BlueprintAssignable)
		FTriggerPressedSigniture OnTriggerPressed;

	UPROPERTY(BlueprintAssignable)
		FTriggerReleasedSigniture OnTriggerReleased;

	UPROPERTY(BlueprintAssignable)
		FGrabEnabledSigniture OnGrabbedEnabled;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UGrabbableComponent* GetGrabbableComponent(AActor* actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static bool HasGrabbableComponent(AActor* actor);
};
