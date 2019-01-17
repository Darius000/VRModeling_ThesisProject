// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"
#include "ConstructorHelpers.h"


// Sets default values
UInteractableComponent::UInteractableComponent()
	:interactedWith(false), interactedCauser(0), canInteractWith(true), maxInterations(-1)
{
	PrimaryComponentTick.bCanEverTick = true;

	bAutoActivate = true;

	SphereRadius = 20.0f;

	CreateMaterialBillboardComponent();
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractableComponent::InteractWith_Implementation(AActor* actor)
{
	interactedCauser = actor;

	if ((maxInterations > -1 && currentInteractions == maxInterations) || !canInteractWith)
	{
		return;
	}

	interactedWith = true;

	OnInteract.Broadcast(actor);
}

bool UInteractableComponent::WasInteractedWith_Implementation()
{
	return interactedWith;
}

AActor* UInteractableComponent::GetInteractionCauser()
{
	return interactedCauser;
}

UInteractableComponent* UInteractableComponent::GetInteractableComponent(AActor* actor)
{
	TArray<UActorComponent*> Components = actor->GetComponentsByClass(UInteractableComponent::StaticClass());

	if(Components.Num() > 0)
		return Cast<UInteractableComponent>(Components[0]);

	return nullptr;
}

bool UInteractableComponent::HasInteractableComponent(AActor* actor)
{
	TArray<UActorComponent*> Components = actor->GetComponentsByClass(UInteractableComponent::StaticClass());

	if (Components.Num() > 0)
	{
		return true;
	}

	return false;
}

void UInteractableComponent::CreateMaterialBillboardComponent()
{
	materialBillboardComponent = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Sprite"));
	materialBillboardComponent->SetupAttachment(this);
	materialBillboardComponent->SetRelativeScale3D(FVector(.3f));
	materialBillboardComponent->bHiddenInGame = false;
}

