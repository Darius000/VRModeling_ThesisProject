// Fill out your copyright notice in the Description page of Project Settings.

#include "GrabbableComponent.h"


UGrabbableComponent::UGrabbableComponent()
	:canGrab(true), grabbed(false)
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGrabbableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGrabbableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UGrabbableComponent::IsGrabbed()
{
	return grabbed;
}

void UGrabbableComponent::CanBeGrabbed_Implementation(bool& IsGrabbableReady)
{
	IsGrabbableReady = canInteractWith && canGrab;
}

void UGrabbableComponent::Grab_Implementation(USceneComponent* attatchTo, FName socket, EControllerHand hand)
{
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	rules.ScaleRule = EAttachmentRule::KeepWorld;
	rules.RotationRule = EAttachmentRule::KeepWorld;

	GetOwner()->AttachToComponent(attatchTo, rules , socket);

	Execute_InteractWith(this, attatchTo->GetOwner());

	OnGrab.Broadcast(attatchTo, socket, hand);

	grabbed = true;
}

void UGrabbableComponent::Release_Implementation()
{
	GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	OnRelease.Broadcast();

	grabbed = false;
}

void UGrabbableComponent::SetGrabbableEnabled_Implementation(bool enabled)
{
	canGrab = enabled;

	OnGrabbedEnabled.Broadcast(enabled);
}

void UGrabbableComponent::SetOutline_Implementation(bool inEnable, int32 inStencil)
{
	OnSetOutline.Broadcast(inEnable, inStencil);
}

void UGrabbableComponent::TriggerPressed_Implementation()
{
	OnTriggerPressed.Broadcast();
}

void UGrabbableComponent::TriggerReleased_Implementation()
{
	OnTriggerReleased.Broadcast();
}

UGrabbableComponent* UGrabbableComponent::GetGrabbableComponent(AActor* actor)
{
	TArray<UActorComponent*> Components = actor->GetComponentsByClass(UGrabbableComponent::StaticClass());

	if(Components.Num() > 0)
		return Cast<UGrabbableComponent>(Components[0]);	

	return nullptr;
}

bool UGrabbableComponent::HasGrabbableComponent(AActor* actor)
{
	TArray<UActorComponent*> Components = actor->GetComponentsByClass(UGrabbableComponent::StaticClass());

	if (Components.Num() > 0)
	{
		return true;
	}

	return false;
}
