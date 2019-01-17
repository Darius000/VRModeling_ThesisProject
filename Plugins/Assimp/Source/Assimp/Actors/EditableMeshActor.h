// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameframework/Actor.h"
#include "Assimp/Components/EditableMeshComponent.h"
#include "Interactions/Components/GrabbableComponent.h"
#include "EditableMeshActor.generated.h"

USTRUCT(BlueprintType)
struct FData
{
	GENERATED_BODY()

	FData()
		:width(100.0f), length(100.0f), size(100.0f), innerSize(50.0f), divisions(12), height(100.0f)
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float length;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float innerSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 divisions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float height;

	/*Used when the shape option is set to custom*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString importPath;

};

/**
 * 
 */
UCLASS()
class ASSIMP_API AEditableMeshActor : public AActor
{
	GENERATED_BODY()

public:

	AEditableMeshActor();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UEditableMeshComponent* EditableMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UGrabbableComponent* GrabbableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ExposeOnSpawn))
		EShape Shape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ExposeOnSpawn))
		FData DefaultData;

private:
	void CreateShape(FData data);

	UFUNCTION()
		void Grab(USceneComponent* attatchTo, FName socket, EControllerHand hand);

protected:
	UFUNCTION()
		void RemoveAsCurrentMeshComponent(AActor* destroyedActor);
	
};
