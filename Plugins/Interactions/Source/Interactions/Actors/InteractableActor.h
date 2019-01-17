// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactions/Interfaces/I_Interact.h"
#include "InteractableActor.generated.h"


UCLASS(ClassGroup = (Interact))
class INTERACTIONS_API AInteractableActor : public AActor, public II_Interact
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

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
};
