// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableActor.h"

// Sets default values
AInteractableActor::AInteractableActor()
	:interactedWith(false), interactedCauser(0), canInteractWith(true), maxInterations(-1)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AInteractableActor::InteractWith_Implementation(AActor* actor)
{
	interactedCauser = actor;

	if ((maxInterations > -1 && currentInteractions == maxInterations) || !canInteractWith)
	{
		return;
	}

	interactedWith = true;

	currentInteractions++;

	//UE_LOG(LogTemp, Log, TEXT("actor :: %s, interacted :: %d, currentInteractions :: %d"), *actor->GetName(), interactedWith, currentInteractions);
}

bool AInteractableActor::WasInteractedWith_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT("interacted :: %d"), interactedWith);

	return interactedWith;
}

AActor* AInteractableActor::GetInteractionCauser()
{
	return interactedCauser;
}

