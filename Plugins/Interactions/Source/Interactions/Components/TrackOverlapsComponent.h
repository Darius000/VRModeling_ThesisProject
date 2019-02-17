// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "TrackOverlapsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddItemSigniture, UObject*, Object);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRemoveItemSigniture, UObject*, Object);

struct FSortByDistance
{
	FSortByDistance()
		:SourceLocation(FVector(0.0f))
	{

	}

	FSortByDistance(const FVector& InSourceLocation)
		:SourceLocation(InSourceLocation)
	{

	}

	//Location to sort the comparison
	FVector SourceLocation;

	bool operator()(const AActor* A, const AActor* B) const
	{
		float distanceA = FVector::DistSquared(SourceLocation, A->GetActorLocation());

		float distanceB = FVector::DistSquared(SourceLocation, B->GetActorLocation());

		return distanceA < distanceB;
	}

	bool operator()(const USceneComponent* A, const USceneComponent* B) const
	{
		float distanceA = FVector::DistSquared(SourceLocation, A->GetComponentLocation());

		float distanceB = FVector::DistSquared(SourceLocation, B->GetComponentLocation());

		return distanceA < distanceB;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIONS_API UTrackOverlapsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTrackOverlapsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;


protected:
	TArray<UPrimitiveComponent*> OverlappedComponents;

	TArray<AActor*> OverlappedActors;

private:
	void FindComponentByName();

	UFUNCTION()
		void AddOverlappedItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool FromSweep,const FHitResult& SweepResult);

	UFUNCTION()
		void RemoveOverlappedItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

protected:
	UFUNCTION(BlueprintCallable)
		virtual void OnAddActor(AActor* actor);

	UFUNCTION(BlueprintCallable)
		virtual void OnRemoveActor(AActor* actor);

	UFUNCTION(BlueprintCallable)
		virtual void OnAddComponent(UPrimitiveComponent* component);

	UFUNCTION(BlueprintCallable)
		virtual void OnRemoveComponent(UPrimitiveComponent* component);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPrimitiveComponent* TestOverlappingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString OverlappingComponentName;
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetFirstActor(AActor*& actor, bool& valid);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetFirstComponent(UPrimitiveComponent*& component, bool& valid);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetNearestActor(AActor*& actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetNearestComponent(UPrimitiveComponent*& component);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsThereActorOverlaps() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsThereComponentOverlaps() const;

	UFUNCTION(BlueprintCallable)
		void SetOverlappedActors(TArray<AActor*>& actors);

	UFUNCTION(BlueprintCallable)
		void SetOverlappedComponents(TArray<UPrimitiveComponent*>& components);

	UPROPERTY(BlueprintAssignable)
		FAddItemSigniture OnItemAdded;

	UPROPERTY(BlueprintAssignable)
		FRemoveItemSigniture OnItemRemoved;
};
