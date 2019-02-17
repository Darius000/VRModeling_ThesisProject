// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssimpComponent.h"
#include "GrabbableBoxComponent.h"
#include "EditableMeshComponent.generated.h"


class FEditableMeshSceneProxy;
class UEditableMeshComponent;


UENUM(BlueprintType)
enum class EEditMode : uint8
{
	OBJECT,
	VERTEX,
	EDGE,
	FACE
};

UENUM(BlueprintType)
enum class EMeshElement : uint8
{
	VERTEX,
	EDGE,
	FACE
};

UENUM(BlueprintType)
enum class EFallOff : uint8
{
	NONE,
	CONSTANT,
	LINEAR,
	QUADRATIC
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEditModeSigniture, EEditMode, EditMode);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMeshModifiedSigniture, UMesh*, MeshState, FString, Message);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateMeshElementSigniture, EEditMode, EditMode);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectedSigniture);

/**
 A primitive component that allows real-time editing of a mesh 
 */
UCLASS(ClassGroup = (Mesh), meta = (BlueprintSpawnableComponent))
class ASSIMP_API UEditableMeshComponent : public UAssimpMeshComponent
{
	GENERATED_BODY()

public:

	UEditableMeshComponent();

	virtual void BeginPlay() override;

	UPROPERTY()
		EEditMode CurrentEditMode;

	UPROPERTY()
		EFallOff CurrentFallOff;

	UPROPERTY()
		float FallOffStrength;

	UFUNCTION(BlueprintCallable)
		void SetFallOff(EFallOff falloff, float strength);

	UFUNCTION(BlueprintCallable)
		void SetEditMode(EEditMode EditMode);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		EEditMode GetCurrentEditMode();

	/*Procedural Shapes*/
	UFUNCTION(BlueprintCallable)
		void Plane(float width = 100.0f, float length = 100.0f);

	UFUNCTION(BlueprintCallable)
		void RectangularPrism(float width = 100.0f, float length = 100.0f, float height = 100.0f);

	UFUNCTION(BlueprintCallable)
		void Cube(float size = 100.0f);

	UFUNCTION(BlueprintCallable)
		void Pyramid(float base = 100.0f, float height = 100.0f);

	UFUNCTION(BlueprintCallable)
		void Disk(float radius = 100.0f, int32 divisions = 12, float offset = 0.0f);

	UFUNCTION(BlueprintCallable)
		void Cylinder(float radius = 100.0f, float height = 100.0f, int32 divisions = 12);

	UFUNCTION(BlueprintCallable)
		void Pipe(float radius = 100.0f, float innerRadius = 50.0f, float height = 100.0f, int32 divisions = 12);

	UFUNCTION(BlueprintCallable)
		void Cone(float radius = 100.0f, float height = 100.0f, int32 divisions = 12);

	UFUNCTION(BlueprintCallable)
		void Octohedren(float radius = 100.0f);

	UFUNCTION(BlueprintCallable)
		void Tetrahedron(float radius = 100.0f);

	UFUNCTION(BlueprintCallable)
		void Sphere(float radius = 100.0f, int32 divisions = 12);

	UFUNCTION(BlueprintCallable)
		void Torus(float radius = 100.0f, float innerRadius = 50.0f, int32 divisons = 12);

	/*Mesh Actions*/
public:
	UFUNCTION(BlueprintCallable)
		void ToggleVertexSelection(int32 meshIndex, int32 index);

	UFUNCTION(BlueprintCallable)
		void ToggleEdgeSelection(int32 meshIndex, int32 index);

	UFUNCTION(BlueprintCallable)
		void ToggleFaceSelection(int32 meshIndex, int32 index);

	UFUNCTION(BlueprintCallable)
		void ToggleSelectAllElementsByType(EMeshElement ElementType);

	UFUNCTION(BlueprintCallable)
		void SelectBorderEdges();

	UFUNCTION(BlueprintCallable)
		void SelectAdjacentEdgesAndFaces();

	UFUNCTION(BlueprintCallable)
		void DeleteSelectedFaces();

	UFUNCTION(BlueprintCallable)
		UIHalfEdge* ExtrudeEdge(UIHalfEdge* Edge, FVector Direction, float Amount, bool bNormal, bool bUpdateScene = false);

	UFUNCTION(BlueprintCallable)
		void ExtrudeEdges(FVector Direction, float Amount, bool bNormal);

	UFUNCTION(BlueprintCallable)
		void ExtrudeFace(UIFace* face, float amount, bool updateScene = false);

	UFUNCTION(BlueprintCallable)
		void ExtrudeFaces(float Amount);

	UFUNCTION(BlueprintCallable)
		void MergeVertex(TArray<UIVertex*> vertices, bool updateScene = false);

	UFUNCTION(BlueprintCallable)
		void MergeVertices();

	UFUNCTION(BlueprintCallable)
		void MakeFace();

	UFUNCTION(BlueprintCallable)
		void MakeVertex(FVector location);

	UFUNCTION(BlueprintCallable)
		void DeleteVertex();

	UFUNCTION()
		void MoveSelectedElements(UGrabbableBoxComponent* component, FVector location);

	UFUNCTION()
		void ApplyFalloff(UGrabbableBoxComponent* component, FVector location);

public:
	UPROPERTY(BlueprintAssignable)
		FEditModeSigniture OnEditModeChanged;

	virtual void UpdateSceneProxy() override;

private:

	UGrabbableBoxComponent* CreateGBComponent(UIElement* element,const FVector Location);

	void SpawnGBComponents(bool forceSpawn = false);

	/*Display Mesh Elements on the draw interface*/
	virtual void DrawMeshComponents(FPrimitiveDrawInterface* PDI) override;

	void DestroyPrevoiusGBComponents();

	void UpdateComponentLocations();

private:

	TArray<UGrabbableBoxComponent*> GrabbableBoxComponents;

	EEditMode PreviousEditMode;
};

