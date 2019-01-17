// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "Components/MaterialBillboardComponent.h"
#include "Interactions/Interfaces/I_Interact.h"
#include "InteractableComponent.generated.h"

UCLASS(ClassGroup = (Interact), meta = (BlueprintSpawnableComponent))
class INTERACTIONS_API UInteractableComponent : public USphereComponent, public II_Interact
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UInteractableComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	AActor* interactedCauser;

	bool interactedWith;

	int32 currentInteractions;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool canInteractWith;

	//less than 0 is unlimited interactions, default is -1
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 maxInterations;

public:
		virtual void InteractWith_Implementation(AActor* actor) override;

		virtual bool WasInteractedWith_Implementation() override;

	UFUNCTION(BlueprintCallable)
		AActor* GetInteractionCauser();

	UPROPERTY(BlueprintAssignable)
		FInteractSigniture OnInteract;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UInteractableComponent* GetInteractableComponent(AActor* actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static bool HasInteractableComponent(AActor* actor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UMaterialBillboardComponent* materialBillboardComponent;

	UFUNCTION()
		virtual void CreateMaterialBillboardComponent();
};

