// Fill out your copyright notice in the Description page of Project Settings.

#include "AssimpTrackOverlapsComponent.h"
#include "Assimp/Public/EditableMeshLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


void UAssimpTrackOverlapsComponent::OnAddComponent(UPrimitiveComponent* component)
{
	if (UEditableMeshLibrary::GetCurrentEditableMeshComponent())
	{
		if (UEditableMeshLibrary::GetCurrentEditableMeshComponent()->GetCurrentEditMode() == EEditMode::OBJECT
			&& Cast<UGrabbableComponent>(component))
		{
			OverlappedComponents.AddUnique(component);

			OnItemAdded.Broadcast(component);
		}
		else if (UEditableMeshLibrary::GetCurrentEditableMeshComponent()->GetCurrentEditMode() != EEditMode::OBJECT
			&& Cast<UGrabbableBoxComponent>(component))
		{
			OverlappedComponents.AddUnique(component);

			OnItemAdded.Broadcast(component);
		}
	}
	else
	{
		Super::OnAddComponent(component);
	}
}
