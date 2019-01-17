// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Mesh.h"
#include "AssimpMeshLibrary.generated.h"


#define EnumToString(EnumClassName, ValueOfEnum)\
	UAssimpMeshLibrary::GetEnumValueAsString<EnumClassName>\
	(FString(TEXT(#EnumClassName)),(ValueOfEnum))




/**
 * 
 */
UCLASS()
class ASSIMP_API UAssimpMeshLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
	{
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);

		if (!enumPtr)
		{
			return FString("Invalid");
		}

		return enumPtr->GetNameStringByValue((int64)Value);
	}

	static FORCEINLINE float ToRadians(float degrees)
	{
		return degrees * (PI / 180.0f);
	}

	static FORCEINLINE void CreateRing(float radius, float height, uint32 divisions, TArray<FVector>& positions)
	{
		float radians = UAssimpMeshLibrary::ToRadians((360.0f / (float)divisions));

		for (int32 i = 0; i < (int32)divisions; ++i)
		{
			float x = radius * FMath::Cos(radians * i);
			float y = radius * FMath::Sin(radians * i);
			float z = height;

			positions.Add(FVector(x, y, z));
		}
	}

	/*Generate Indices Functions, Number of faces generate is determined by the vertex array size / 3*/

	static FORCEINLINE void Triangles(UIMesh* mesh, TArray<UIVertex*> vertices)
	{
		uint32 Index = 0;

		for (int32 i = 0; i < (vertices.Num() / 3); ++i)
		{
			UIFace* face = UIFace::CreateFace(mesh, vertices[0 + Index], vertices[1 + Index], vertices[2 + Index]);

			Index += 3;
		}
	}

	/*, Number of faces generate is determined by the vertex array size / 4*/

	static FORCEINLINE void Quads(UIMesh* mesh, TArray<UIVertex*> vertices)
	{
		int32 numVertices = vertices.Num();


		for (int32 i = 0, Index = 0; i < (numVertices / 4); ++i)
		{
			UIFace* face = UIFace::CreateFace(mesh, vertices[Index], vertices[Index + 1], vertices[(Index + 2)], vertices[(Index + 3)]);

			//UIFace* face2 = UIFace::CreateFace(mesh, vertices[Index + 2], vertices[Index + 3], vertices[Index]);

			Index += 4;
		}
	}

	/*Row1 and Row2 must have the same number of vertices to create a strip*/
	static FORCEINLINE void QuadStrip(UIMesh* mesh, TArray<UIVertex*> row1, TArray<UIVertex*> row2, bool bClosed = false)
	{
		int32 numVertices = bClosed ? row1.Num() - 1: row1.Num() - 1;

		uint32 Index = 0;

		for (int32 i = 0; i < (bClosed ? numVertices : numVertices - 1); ++i)
		{
			UIFace* face = UIFace::CreateFace(mesh,
				row1[i			% numVertices],
				row2[i			% numVertices],
				row2[(i + 1)	% numVertices],
				row1[(i + 1)	% numVertices]
			);

			/*UIFace* face2 = UIFace::CreateFace(mesh,
				row2[(i + 1)	% numVertices],
				row1[(i + 1)	% numVertices],
				row1[i			% numVertices]
			);*/

			Index += 4;
		}
	}

	static FORCEINLINE void TriangleFan(UIMesh* mesh, TArray<UIVertex*> vertices, const uint32 numFaces)
	{
		int32 numVertices = vertices.Num();

		for (int32 i = 0; i < (int32)(numFaces - 2); ++i)
		{
			UIFace* face = UIFace::CreateFace(mesh, vertices[(i % (numVertices - 1))], vertices[((i + 1) % (numVertices - 1))], vertices[(numVertices - 1)]);
		}
	}

};
