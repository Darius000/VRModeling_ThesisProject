// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactions/Components/TrackOverlapsComponent.h"
#include "AssimpTrackOverlapsComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ASSIMP_API UAssimpTrackOverlapsComponent : public UTrackOverlapsComponent
{
	GENERATED_BODY()
	
protected:
	virtual void OnAddComponent(UPrimitiveComponent* component) override;
};
