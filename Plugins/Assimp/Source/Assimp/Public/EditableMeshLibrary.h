// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Assimp/Components/EditableMeshComponent.h"
#include "Mesh.h"
#include "EditableMeshLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ASSIMP_API UEditableMeshLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UEditableMeshComponent* GetCurrentEditableMeshComponent();

	UFUNCTION(BlueprintCallable)
	static void SetCurrentEditableMeshComponent(UEditableMeshComponent* EditableMeshComponent);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static TArray<UIVertex*> GetSelectedVertices();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static TArray<UIHalfEdge*> GetSelectedEdges();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static TArray<UIFace*> GetSelectedFaces();

	UFUNCTION(BlueprintCallable)
		static void AddVerticesToSelected(TArray<UIVertex*> vertices);

	UFUNCTION(BlueprintCallable)
		static void AddEdgesToSelected(TArray<UIHalfEdge*> edges);

	UFUNCTION(BlueprintCallable)
		static void AddFacesToSelected(TArray<UIFace*> faces);

	UFUNCTION(BlueprintCallable)
		static void RemoveVerticesFromSelected(TArray<UIVertex*> vertices);

	UFUNCTION(BlueprintCallable)
		static void RemoveEdgesFromSelected(TArray<UIHalfEdge*> edges);

	UFUNCTION(BlueprintCallable)
		static void RemoveFacesFromSelected(TArray<UIFace*> faces);

private:

	static UEditableMeshComponent* CurrentEditableMeshComponent;

private:
	static TArray<UIFace*> mSelectedFaces;

	static TArray<UIHalfEdge*> mSelectedEdges;

	static TArray<UIVertex*> mSelectedVertices;
};
