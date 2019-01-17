#include "Tangents.h"

FITangents::FITangents()
{
	Normal = FVector();

	SetTangentAndBiNormal();
}

FITangents::FITangents(FVector InNormal)
{
	Normal = InNormal;

	SetTangentAndBiNormal();
}

void FITangents::SetNormal(FVector InNormal)
{
	Normal = InNormal;
}

void FITangents::SetTangent(FVector InTangent)
{
	Tangent = InTangent;
}

void FITangents::SetBiNormal(FVector InBiNormal)
{
	BiNormal = InBiNormal;
}

FVector FITangents::GetNormal() const
{
	return Normal;
}

FVector FITangents::GetTangent() const
{
	return Tangent;
}

FVector FITangents::GetBiNormal() const
{
	return BiNormal;
}

void FITangents::SetTangentAndBiNormal()
{
	FVector c1 = Normal ^ FVector::UpVector;
	FVector c2 = Normal ^ FVector::RightVector;

	Tangent = (c1.Size() > c2.Size()) ? c1 : c2;

	BiNormal = Normal ^ Tangent;
}

bool FITangents::operator==(const FITangents& tangent) const
{
	return Normal == tangent.Normal &&
		Tangent == tangent.Tangent &&
		BiNormal == tangent.BiNormal;
}
