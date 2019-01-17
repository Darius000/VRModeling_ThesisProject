#pragma once

#include "CoreMinimal.h"
#include "Tangents.generated.h"

USTRUCT(BlueprintType)
struct ASSIMP_API FITangents
{
	GENERATED_BODY()

		FITangents();

	/*Construct a tangent with the normal angle given: tangents and binormal calculated after input*/
	FITangents(FVector InNormal);

	/*Set the normal vector*/
	void SetNormal(FVector InNormal);

	void SetTangent(FVector InTangent);

	void SetBiNormal(FVector InBiNormal);

	/*returns the normal vector direction*/
	FVector GetNormal() const;

	/*returns the tangent vector direction*/
	FVector GetTangent() const;

	/*returns the binormal vector direction*/
	FVector GetBiNormal() const;

	/*Set the tangents and binormal given a normal*/
	void SetTangentAndBiNormal();

	bool operator==(const FITangents& tangent) const;

private:

	FVector Normal;

	FVector Tangent;

	FVector BiNormal;
};