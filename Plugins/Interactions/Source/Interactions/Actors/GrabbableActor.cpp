// Fill out your copyright notice in the Description page of Project Settings.

#include "GrabbableActor.h"


AGrabbableActor::AGrabbableActor()
	:canGrab(true)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGrabbableActor::CanBeGrabbed_Implementation(bool& IsGrabbableReady)
{
	IsGrabbableReady = canInteractWith && canGrab;

	//UE_LOG(LogTemp, Log, TEXT("Set Grabbed Ready"));
}

void AGrabbableActor::Grab_Implementation(USceneComponent* attatchTo, FName socket, EControllerHand hand)
{
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	rules.ScaleRule = EAttachmentRule::KeepRelative;

	AttachToComponent(attatchTo, rules , socket);

	Execute_InteractWith(this, attatchTo->GetOwner());

	//UE_LOG(LogTemp, Log, TEXT("Grabbed"));
}

void AGrabbableActor::Release_Implementation()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//UE_LOG(LogTemp, Log, TEXT("Released"));
}

void AGrabbableActor::SetGrabbableEnabled_Implementation(bool enabled)
{
	canGrab = enabled;

	//UE_LOG(LogTemp, Log, TEXT("CanGrabbed"));
}

void AGrabbableActor::SetOutline_Implementation(bool inEnable, int32 inStencil)
{
	//UE_LOG(LogTemp, Log, TEXT("Set Outline"));
}

void AGrabbableActor::TriggerPressed_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT("Trigger Pressed"));
}

void AGrabbableActor::TriggerReleased_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT("Trigger Released"));
}
