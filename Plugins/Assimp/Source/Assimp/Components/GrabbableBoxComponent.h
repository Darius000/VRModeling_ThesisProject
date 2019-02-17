// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactions/Components/GrabbableComponent.h"
#include "Components/BillboardComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Assimp/Public/Mesh.h"
#include "GrabbableBoxComponent.generated.h"

#define GRABBABLECOMPONENT_COLLISION ECC_GameTraceChannel1

class UGrabbableBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateSigniture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdatePositionSigniture,UGrabbableBoxComponent*, Component, FVector, Location);
/**
 * 
 */
UCLASS(ClassGroup = (Interact))
class ASSIMP_API UGrabbableBoxComponent : public UGrabbableComponent
{
	GENERATED_BODY()

public:

	UGrabbableBoxComponent();

	~UGrabbableBoxComponent();

private:
	bool mSelected;

	AActor* OwnerActor;

public:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	void SetMeshElememt(UIElement* element);

	UFUNCTION()
	void BeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void EndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Grab_Implementation(USceneComponent* attatchTo, FName socket, EControllerHand hand) override;

	virtual void Release_Implementation() override;

	virtual void TriggerPressed_Implementation() override;

	virtual void TriggerReleased_Implementation() override;

	virtual void CanBeGrabbed_Implementation(bool& IsGrabbableReady) override;

	/*Toggle component selection*/
	UFUNCTION(BlueprintCallable)
		void Select();

	/*Is the component currently selected?*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool Selected();

public:
	UPROPERTY(BlueprintAssignable)
		FUpdateSigniture OnUpdated;

	UPROPERTY(BlueprintAssignable)
		FUpdatePositionSigniture OnPositionUpdated;

	UPROPERTY()
		UIElement* meshElement;
};
