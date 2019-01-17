// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackOverlapsComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interactions/Interfaces/I_Grabbable.h"
#include "Interactions/Interfaces/I_Interact.h"

// Sets default values for this component's properties
UTrackOverlapsComponent::UTrackOverlapsComponent()
	:TestOverlappingComponent(0), OverlappingComponentName("")
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bAutoActivate = true;

	// ...
}


// Called when the game starts
void UTrackOverlapsComponent::BeginPlay()
{
	Super::BeginPlay();

	FindComponentByName();

	// ...	
	if (TestOverlappingComponent)
	{
		TestOverlappingComponent->OnComponentBeginOverlap.AddDynamic(this, &UTrackOverlapsComponent::AddOverlappedItem);
		TestOverlappingComponent->OnComponentEndOverlap.AddDynamic(this, &UTrackOverlapsComponent::RemoveOverlappedItem);
	}
}


// Called every frame
void UTrackOverlapsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//void UTrackOverlapsComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//
//	if (PropertyChangedEvent.GetPropertyName() == "OverlappingComponentName")
//	{
//		FindComponentByName();
//	}
//}

void UTrackOverlapsComponent::FindComponentByName()
{
	if (GetOwner())
	{
		TArray<UActorComponent*> components = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());

		for (int32 i = 0; i < components.Num(); ++i)
		{
			if (components[i]->GetName() == OverlappingComponentName)
			{
				TestOverlappingComponent = Cast<UPrimitiveComponent>(components[i]);

				break;
			}
		}
	}

	return;
}

void UTrackOverlapsComponent::AddOverlappedItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UI_Interact::StaticClass()))
	{
		//Future use
	}

	if (UKismetSystemLibrary::DoesImplementInterface(OtherComponent, UI_Interact::StaticClass()))
	{
		//Future use
	}

	if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UI_Grabbable::StaticClass()))
	{
		OverlappedActors.AddUnique(OtherActor);

		OnItemAdded.Broadcast(OtherActor);
	}

	if (UKismetSystemLibrary::DoesImplementInterface(OtherComponent, UI_Grabbable::StaticClass()))
	{
		OverlappedComponents.AddUnique(OtherComponent);

		OnItemAdded.Broadcast(OtherComponent);
	}
}

void UTrackOverlapsComponent::RemoveOverlappedItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UI_Interact::StaticClass()))
	{
		//Future use
	}

	if (UKismetSystemLibrary::DoesImplementInterface(OtherComponent, UI_Interact::StaticClass()))
	{
		//Future use
	}

	if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UI_Grabbable::StaticClass()))
	{
		OverlappedActors.Remove(OtherActor);

		OnItemRemoved.Broadcast(OtherActor);
	}

	if (UKismetSystemLibrary::DoesImplementInterface(OtherComponent, UI_Grabbable::StaticClass()))
	{
		OverlappedComponents.Remove(OtherComponent);

		OnItemRemoved.Broadcast(OtherComponent);
	}
}

void UTrackOverlapsComponent::GetFirstActor(AActor*& actor, bool& valid)
{
	if (IsThereActorOverlaps())
	{
		valid = true;

		actor = OverlappedActors[0];

		return;
	}

	actor = nullptr;

	valid = false;

	return;
}

void UTrackOverlapsComponent::GetFirstComponent(UPrimitiveComponent*& component, bool& valid)
{
	if (IsThereComponentOverlaps())
	{
		valid = true;

		component = OverlappedComponents[0];

		return;
	}

	component = nullptr;

	valid = false;

	return;
}

void UTrackOverlapsComponent::GetNearestActor(AActor*& actor)
{
	if (IsThereActorOverlaps())
	{
		Algo::Sort(OverlappedActors, FSortByDistance(TestOverlappingComponent->GetComponentLocation()));

		actor = OverlappedActors[0];
	}
}

void UTrackOverlapsComponent::GetNearestComponent(UPrimitiveComponent*& component)
{
	if (IsThereComponentOverlaps())
	{
		Algo::Sort(OverlappedComponents, FSortByDistance(TestOverlappingComponent->GetComponentLocation()));

		component = OverlappedComponents[0];
	}
}

bool UTrackOverlapsComponent::IsThereActorOverlaps() const
{
	return OverlappedActors.Num() > 0;
}

bool UTrackOverlapsComponent::IsThereComponentOverlaps() const
{
	return OverlappedComponents.Num() > 0;
}

void UTrackOverlapsComponent::SetOverlappedActors(TArray<AActor*>& actors)
{
	OverlappedActors = actors;

	return;
}

void UTrackOverlapsComponent::SetOverlappedComponents(TArray<UPrimitiveComponent*>& components)
{
	OverlappedComponents = components;

	return;
}

