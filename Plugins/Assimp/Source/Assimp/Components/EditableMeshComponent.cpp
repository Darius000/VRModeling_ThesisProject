// Fill out your copyright notice in the Description page of Project Settings.

#include "EditableMeshComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Assimp/Public/AssimpMeshLibrary.h"
#include "GrabbableBoxComponent.h"
#include "kismet/KismetMathLibrary.h"
#include "Assimp/Public/EditableMeshLibrary.h"

UEditableMeshComponent::UEditableMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEditableMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEditableMeshComponent::SetFallOff(EFallOff falloff, float strength)
{
	CurrentFallOff = falloff;
	FallOffStrength = strength;
}

void UEditableMeshComponent::SetEditMode(EEditMode EditMode)
{
	PreviousEditMode = CurrentEditMode;

	CurrentEditMode = EditMode;

	OnEditModeChanged.Broadcast(CurrentEditMode);

	switch (CurrentEditMode)
	{
	case EEditMode::OBJECT:
		bDrawVertices	= false;
		bDrawEdges		= false;
		bDrawFaces		= false;
		break;
	case EEditMode::VERTEX:
		bDrawVertices = true;
		bDrawEdges = false;
		bDrawFaces = false;
		break;
	case EEditMode::EDGE:
		bDrawVertices = false;
		bDrawEdges = true;
		bDrawFaces = false;
		break;
	case EEditMode::FACE:
		bDrawVertices = false;
		bDrawEdges = false;
		bDrawFaces = true;
		break;
	default:
		break;
	}

	SpawnGBComponents();
}


EEditMode UEditableMeshComponent::GetCurrentEditMode()
{
	return CurrentEditMode;
}

UGrabbableBoxComponent* UEditableMeshComponent::CreateGBComponent(UIElement* element, const FVector Location)
{
	UGrabbableBoxComponent* NewComponent = NewObject<UGrabbableBoxComponent>(this, UGrabbableBoxComponent::StaticClass());

	if (!NewComponent)
	{
		return NULL;
	}

	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

	NewComponent->RegisterComponent();

	NewComponent->AttachToComponent(GetOwner()->GetRootComponent(), rules );
	NewComponent->SetRelativeLocation(Location);
	NewComponent->OnUpdated.AddDynamic(this, &UEditableMeshComponent::UpdateSceneProxy);
	NewComponent->OnPositionUpdated.AddDynamic(this, &UEditableMeshComponent::ApplyFalloff);
	NewComponent->OnPositionUpdated.AddDynamic(this, &UEditableMeshComponent::MoveSelectedElements);
	NewComponent->SetMeshElememt(element);

	return NewComponent;
}

void UEditableMeshComponent::SpawnGBComponents(bool forceSpawn)
{
	if (CurrentEditMode != PreviousEditMode || forceSpawn)
	{
		DestroyPrevoiusGBComponents();

		int32 edgeIndex = 0;

		int32 totalElements =
			CurrentEditMode == EEditMode::EDGE ? mCurrentMesh->mEdges.Num() :
			CurrentEditMode == EEditMode::VERTEX ? mCurrentMesh->mVertices.Num() :
			CurrentEditMode == EEditMode::FACE ? mCurrentMesh->mFaces.Num() : 0;

		GrabbableBoxComponents.AddUninitialized(totalElements);

		for (int32 i = 0; i < totalElements; ++i)
		{
			UIElement* element = 0;

			if (CurrentEditMode == EEditMode::EDGE)
				element = mCurrentMesh->mEdges[i];
			else if (CurrentEditMode == EEditMode::VERTEX)
				element = mCurrentMesh->mVertices[i];
			else if (CurrentEditMode == EEditMode::FACE)
				element = mCurrentMesh->mFaces[i];

			GrabbableBoxComponents[edgeIndex] = CreateGBComponent(element, element->GetPosition());
			edgeIndex++;
		}
	}
}


void UEditableMeshComponent::DrawMeshComponents(FPrimitiveDrawInterface* PDI)
{
	Super::DrawMeshComponents(PDI);
}


void UEditableMeshComponent::DestroyPrevoiusGBComponents()
{
	for (UGrabbableBoxComponent* grabbableBoxComponent : GrabbableBoxComponents)
	{
		//Deselect component
		if (grabbableBoxComponent->Selected())
		{			
			grabbableBoxComponent->Select();
		}

		grabbableBoxComponent->meshElement->SetGrabbableBoxComponent(nullptr);
		grabbableBoxComponent->SetMeshElememt(nullptr);
		grabbableBoxComponent->DestroyComponent();	
	}

	GrabbableBoxComponents.SetNum(0);
}

void UEditableMeshComponent::UpdateComponentLocations()
{
	for (UGrabbableBoxComponent* grabbableBoxComponent : GrabbableBoxComponents)
	{
		if (grabbableBoxComponent->meshElement->GetPosition() != grabbableBoxComponent->GetRelativeTransform().GetLocation() && !grabbableBoxComponent->IsGrabbed()) 
			grabbableBoxComponent->SetRelativeLocation(grabbableBoxComponent->meshElement->GetPosition());
	}
}

void UEditableMeshComponent::Plane(float width, float length)
{
	width /= 2.0f;
	length /= 2.0f;

	UIMesh* mesh = UIMesh::CreateMesh(this);
	
	UIVertex* V0 = UIVertex::CreateVertex(mesh, FVector(-width, -length, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UIVertex* V1 = UIVertex::CreateVertex(mesh, FVector(width, -length, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 0.0f));

	UIVertex* V2 = UIVertex::CreateVertex(mesh, FVector(width, length, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 1.0f));

	UIVertex* V3 = UIVertex::CreateVertex(mesh, FVector(-width, length, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 1.0f));

	UAssimpMeshLibrary::Quads(mesh, { V0, V1, V2, V3 });

	AddMesh(mesh);

	SelectMesh(0);
	
	UpdateSceneProxy();
}

void UEditableMeshComponent::RectangularPrism(float width, float length, float height)
{
	width /= 2.0f;
	length /= 2.0f;

	UIMesh* mesh = UIMesh::CreateMesh(this);

	UIVertex* V0 = UIVertex::CreateVertex(mesh, FVector(-width, -length, height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UIVertex* V1 = UIVertex::CreateVertex(mesh, FVector(width, -length, height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 0.0f));

	UIVertex* V2 = UIVertex::CreateVertex(mesh, FVector(width, length, height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 1.0f));

	UIVertex* V3 = UIVertex::CreateVertex(mesh, FVector(-width, length, height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 1.0f));

	UIVertex* V4 = UIVertex::CreateVertex(mesh, FVector(-width, -length, -height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UIVertex* V5 = UIVertex::CreateVertex(mesh, FVector(width, -length, -height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 0.0f));

	UIVertex* V6 = UIVertex::CreateVertex(mesh, FVector(width, length, -height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 1.0f));

	UIVertex* V7 = UIVertex::CreateVertex(mesh, FVector(-width, length, -height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 1.0f));

	UAssimpMeshLibrary::Quads(mesh,
		{ 
			V0, V1, V2, V3, 
			V4, V5, V6, V7,
			V1, V2, V6, V5,
			V0, V4, V7, V3,
			V0, V1, V5, V4,
			V2, V3, V7, V6
		}
	);

	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Cube(float size)
{
	RectangularPrism(size, size, size);
}

void UEditableMeshComponent::Pyramid(float base, float height)
{
	base /= 2.0f;

	UIMesh* mesh = NewObject<UIMesh>(this, UIMesh::StaticClass());

	UIVertex* V0 = UIVertex::CreateVertex(mesh, FVector(-base, -base, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UIVertex* V1 = UIVertex::CreateVertex(mesh, FVector(base, -base, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 0.0f));

	UIVertex* V2 = UIVertex::CreateVertex(mesh, FVector(base, base, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(1.0f, 1.0f));

	UIVertex* V3 = UIVertex::CreateVertex(mesh, FVector(-base, base, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 1.0f));

	UIVertex* V4 = UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, height), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UAssimpMeshLibrary::Quads(mesh, { V0, V1, V2, V3 });
	UAssimpMeshLibrary::Triangles(mesh, {V0, V1, V4, V1, V2, V4, V2, V3, V4, V3, V0, V4});

	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Disk(float radius , int32 divisions, float offset)
{
	UIMesh* mesh = UIMesh::CreateMesh(this);

	TArray<UIVertex*> vertices;
	vertices.SetNum(divisions);
	TArray<FVector> positions;

	UAssimpMeshLibrary::CreateRing(radius, offset, divisions, positions);

	for (int32 i = 0; i < positions.Num(); ++i)
	{
		FVector2D uv = FVector2D(positions[i].X, positions[i].Y);
		uv.Normalize();
		vertices[i] = UIVertex::CreateVertex(mesh, positions[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}

	//-----------------Add Center----------------------->
	vertices.Add(UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, offset), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f)));

	//divisions + 2 for the extra vertex and another +1 to close the beginning and end vertex
	UAssimpMeshLibrary::TriangleFan(mesh, vertices, divisions + 2);

	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Cylinder(float radius /*= 100.0f*/, float height /*= 100.0f*/, int32 divisions /*= 12*/)
{
	UIMesh* mesh = UIMesh::CreateMesh(this);

	TArray<UIVertex*> topVertices;
	topVertices.SetNum(divisions + 1);

	TArray<UIVertex*> bottomVertices;
	bottomVertices.SetNum(divisions + 1);

	TArray<FVector> top;
	TArray<FVector> bottom;

	UAssimpMeshLibrary::CreateRing(radius, height / 2.0f, divisions, top);
	UAssimpMeshLibrary::CreateRing(radius, -height / 2.0f, divisions, bottom);

	for (int32 i = 0; i < top.Num(); ++i)
	{
		FVector2D uv = FVector2D(top[i].X, top[i].Y);
		uv.Normalize();
		topVertices[i] = UIVertex::CreateVertex(mesh, top[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}

	for (int32 i = 0; i < bottom.Num(); ++i)
	{
		FVector2D uv = FVector2D(bottom[i].X, bottom[i].Y);
		uv.Normalize();
		bottomVertices[i] = UIVertex::CreateVertex(mesh, bottom[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}
	//----------------------Add Sides--------------------------------->

	UAssimpMeshLibrary::QuadStrip(mesh, topVertices, bottomVertices, true);

	//-----------------Add Base Center-----------------------Create Base>
	topVertices[divisions] = UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UAssimpMeshLibrary::TriangleFan(mesh, topVertices, divisions + 2);

	//----------------Add Center Height---------------------- Create Top>
	bottomVertices[divisions] = UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, -height / 2.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UAssimpMeshLibrary::TriangleFan(mesh, bottomVertices, divisions + 2);

	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Pipe(float radius /*= 100.0f*/, float innerRadius /*= 50.0f*/, float height /*= 100.0f*/, int32 divisions /*= 12*/)
{
	UIMesh* mesh = UIMesh::CreateMesh(this);

	TArray<UIVertex*> topVertices;
	topVertices.SetNum(divisions);

	TArray<UIVertex*> bottomVertices;
	bottomVertices.SetNum(divisions);

	TArray<UIVertex*> topInnerVertices;
	topInnerVertices.SetNum(divisions);

	TArray<UIVertex*> bottomInnerVertices;
	bottomInnerVertices.SetNum(divisions);

	TArray<FVector> top;
	TArray<FVector> bottom;
	TArray<FVector> topInner;
	TArray<FVector> bottomInner;

	UAssimpMeshLibrary::CreateRing(radius, height / 2.0f, divisions, top);
	UAssimpMeshLibrary::CreateRing(radius, -height / 2.0f, divisions, bottom);
	UAssimpMeshLibrary::CreateRing(innerRadius, height / 2.0f, divisions, topInner);
	UAssimpMeshLibrary::CreateRing(innerRadius, -height / 2.0f, divisions, bottomInner);

	for (int32 i = 0; i < top.Num(); ++i)
	{
		FVector2D uv = FVector2D(top[i].X, top[i].Y);
		uv.Normalize();
		topVertices[i] = UIVertex::CreateVertex(mesh, top[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}

	for (int32 i = 0; i < bottom.Num(); ++i)
	{
		FVector2D uv = FVector2D(bottom[i].X, bottom[i].Y);
		uv.Normalize();
		bottomVertices[i] = UIVertex::CreateVertex(mesh, bottom[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}

	for (int32 i = 0; i < topInner.Num(); ++i)
	{
		FVector2D uv = FVector2D(topInner[i].X, topInner[i].Y);
		uv.Normalize();
		topInnerVertices[i] = UIVertex::CreateVertex(mesh, topInner[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}

	for (int32 i = 0; i < bottomInner.Num(); ++i)
	{
		FVector2D uv = FVector2D(bottomInner[i].X, bottomInner[i].Y);
		uv.Normalize();
		bottomInnerVertices[i] = UIVertex::CreateVertex(mesh, bottomInner[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}
	//----------------------Add Sides--------------------------------->

	UAssimpMeshLibrary::QuadStrip(mesh, topVertices, bottomVertices, true);
	UAssimpMeshLibrary::QuadStrip(mesh, topInnerVertices, bottomInnerVertices, true);

	UAssimpMeshLibrary::QuadStrip(mesh, topVertices, topInnerVertices, true);
	UAssimpMeshLibrary::QuadStrip(mesh, bottomVertices, bottomInnerVertices, true);

	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Cone(float radius , float height , int32 divisions )
{
	UIMesh* mesh = UIMesh::CreateMesh(this);

	TArray<UIVertex*> vertices;
	vertices.SetNum(divisions + 1);
	TArray<FVector> positions;

	UAssimpMeshLibrary::CreateRing(radius, 0, divisions, positions);

	for (int32 i = 0; i < positions.Num(); ++i)
	{
		FVector2D uv = FVector2D(positions[i].X, positions[i].Y);
		uv.Normalize();
		vertices[i] = UIVertex::CreateVertex(mesh, positions[i], FVector(0.0f, 0.0f, -1.0f), uv);
	}

	//uint32 index = 0;

	//-----------------Add Base Center-----------------------Create Base>
	vertices[divisions] = UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UAssimpMeshLibrary::TriangleFan(mesh, vertices, divisions + 2);
	
	//----------------Add Center Height---------------------- Create Top>
	vertices[divisions] = UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, height), FVector(0.0f, 0.0f, -1.0f), FVector2D(0.0f, 0.0f));

	UAssimpMeshLibrary::TriangleFan(mesh, vertices, divisions + 2);


	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Octohedren(float radius /*= 100.0f*/)
{
	UIMesh* mesh = UIMesh::CreateMesh(this);

	TArray<FVector> positions;
	
	UAssimpMeshLibrary::CreateRing(radius, 0.0f, 4, positions);

	UIVertex* V0 = UIVertex::CreateVertex(mesh, positions[0]);

	UIVertex* V1 = UIVertex::CreateVertex(mesh, positions[1]);

	UIVertex* V2 = UIVertex::CreateVertex(mesh, positions[2]);

	UIVertex* V3 = UIVertex::CreateVertex(mesh, positions[3]);

	UIVertex* V4 = UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, radius));

	UIVertex* V5 = UIVertex::CreateVertex(mesh, FVector(0.0f, 0.0f, -radius));

	UAssimpMeshLibrary::Triangles(mesh,
		{
			V0, V1, V4,
			V1, V2, V4,
			V2, V3, V4,
			V3, V0, V4,
			V0, V1, V5,
			V1, V2, V5,
			V2, V3, V5,
			V3, V0, V5,
		}
	);

	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Tetrahedron(float radius)
{
	UIMesh* mesh = UIMesh::CreateMesh(this);

	TArray<FVector> positions;

	UAssimpMeshLibrary::CreateRing(radius, 0.0f, 3, positions);

	UIVertex* V0 = UIVertex::CreateVertex(mesh, positions[0]);

	UIVertex* V1 = UIVertex::CreateVertex(mesh, positions[1]);

	UIVertex* V2 = UIVertex::CreateVertex(mesh, positions[2]);

	UIVertex* V3 = UIVertex::CreateVertex(mesh ,FVector(0.0f, 0.0f, radius));

	UAssimpMeshLibrary::Triangles(mesh, 
		{
			V0, V1, V2,
			V0, V2, V3,
			V2, V1, V3,
			V1, V0, V3
		});

	AddMesh(mesh);

	SelectMesh(0);

	UpdateSceneProxy();
}

void UEditableMeshComponent::Sphere(float radius , int32 divisions)
{
	FString errorCode = FString("");

	ImportModel(FPaths::ProjectDir() + "/Resources/Shapes/Sphere.obj", errorCode);

	UE_LOG(LogTemp, Error, TEXT("%s"), *errorCode);
}

void UEditableMeshComponent::Torus(float radius /*= 100.0f*/, float innerRadius /*= 50.0f*/, int32 divisons /*= 12*/)
{
	FString errorCode = FString("");

	ImportModel(FPaths::ProjectDir() + "/Resources/Shapes/Torus.obj", errorCode);

	UE_LOG(LogTemp, Error, TEXT("%s"), *errorCode);
}


void UEditableMeshComponent::ToggleVertexSelection(int32 meshIndex, int32 index)
{
	static bool selected = false;

	selected = !selected;

	if (SelectMesh(meshIndex))
	{
		mCurrentMesh->mVertices[index]->Select(selected);
	}
}

void UEditableMeshComponent::ToggleEdgeSelection(int32 meshIndex, int32 index)
{
	static bool selected = false;

	if (SelectMesh(meshIndex))
	{
		selected = !selected;

		mCurrentMesh->mEdges[index]->Select(selected);
	}
}

void UEditableMeshComponent::ToggleFaceSelection(int32 meshIndex, int32 index)
{
	static bool selected = false;

	selected = !selected;

	if (SelectMesh(meshIndex))
	{
		mCurrentMesh->mFaces[index]->Select(selected);
	}
}

void UEditableMeshComponent::ToggleSelectAllElementsByType(EMeshElement ElementType)
{
	switch (ElementType)
	{
	case EMeshElement::VERTEX:
		for (UIVertex* vertex : UEditableMeshLibrary::GetSelectedVertices())
		{
			if (UGrabbableBoxComponent* component = Cast<UGrabbableBoxComponent>(vertex->mGrabbableBoxComponent))
			{
				component->Select();
			}
		}
		break;
	case EMeshElement::EDGE:
		for (UIHalfEdge* edge : UEditableMeshLibrary::GetSelectedEdges())
		{
			if (UGrabbableBoxComponent* component = Cast<UGrabbableBoxComponent>(edge->mGrabbableBoxComponent))
			{
				component->Select();
			}
		}
		break;
	case EMeshElement::FACE:
		for (UIFace* face : UEditableMeshLibrary::GetSelectedFaces())
		{
			if (UGrabbableBoxComponent* component = Cast<UGrabbableBoxComponent>(face->mGrabbableBoxComponent))
			{
				component->Select();
			}
		}
		break;
	default:
		break;
	}
}

void UEditableMeshComponent::SelectBorderEdges()
{
	for (UIHalfEdge* edge : mCurrentMesh->mEdges)
	{
		if (!edge->GetTwinEdge())
		{
			if (UGrabbableBoxComponent* component = Cast<UGrabbableBoxComponent>(edge->mGrabbableBoxComponent))
			{
				if (!component->Selected())
				{
					component->Select();
				}
			}
		}
	}
}

void UEditableMeshComponent::SelectAdjacentEdgesAndFaces()
{
	for (UIVertex* vertex : UEditableMeshLibrary::GetSelectedVertices())
	{
		for (UIHalfEdge* edge : vertex->mConnectedEdges)
		{
			edge->Select(true);
			edge->GetFace()->Select(true);
		}
	}
}

void UEditableMeshComponent::DeleteSelectedFaces()
{
	for (UIFace* face : UEditableMeshLibrary::GetSelectedFaces())
	{
		face->Destroy();
	}

	SpawnGBComponents(true);

	UpdateSceneProxy();
}

UIHalfEdge* UEditableMeshComponent::ExtrudeEdge(UIHalfEdge* Edge, FVector Direction, float Amount, bool bNormal, bool bUpdateScene /*= false*/)
{
	UIMesh* mesh = Edge->GetFace()->GetMesh();

	if (!Edge->GetTwinEdge())
	{
		FVector extrudeDirection = Edge->GetNormal();

		if (!bNormal)
		{
			extrudeDirection = Direction;
		}

		UIVertex* V0 = Edge->GetVertex();
		UIVertex* V1 = Edge->GetNextEdge()->GetVertex();

		FVector NewPosition1 = V0->GetPosition() + extrudeDirection * Amount;
		FVector NewPosition2 = V1->GetPosition() + extrudeDirection * Amount;

		UIVertex* V4 = UIVertex::CreateVertex(mesh, NewPosition1);
		UIVertex* V5 = UIVertex::CreateVertex(mesh, NewPosition2);

		UAssimpMeshLibrary::Quads(mesh, { V4, V5, V1, V0 });

		Edge->GetFace()->GetMesh()->CreateEdgeTwins();

		if (bUpdateScene)
		{
			UpdateSceneProxy();
		}
	}

	return nullptr;
}

void UEditableMeshComponent::ExtrudeEdges(FVector Direction, float Amount, bool bNormal)
{
	for (UIHalfEdge* Edge : UEditableMeshLibrary::GetSelectedEdges())
	{
		ExtrudeEdge(Edge, Direction, Amount, bNormal);
		Edge->Select(false);
	}

	SpawnGBComponents(true);

	UpdateSceneProxy();
}

void UEditableMeshComponent::ExtrudeFace(UIFace* face, float amount, bool updateScene)
{	
	TArray<UIVertex*> CVertices;
	UIMesh* mesh = face->GetMesh();

	bool bDestroyFaceWhenFinished = false;

	UIHalfEdge* edge = face->GetEdge();

	do
	{
		CVertices.Add(edge->GetVertex());

		if (edge->GetTwinEdge())
			bDestroyFaceWhenFinished = true;

		edge = edge->GetNextEdge();

	} while (edge != face->GetEdge());

	edge = face->GetEdge();

	do
	{
		FVector NewLocation = edge->GetVertex()->GetPosition() + (-face->CalculateNormal() * amount);

		CVertices.Add(UIVertex::CreateVertex(mesh, NewLocation));

		edge = edge->GetNextEdge();

	} while (edge != face->GetEdge());


	UIVertex* V0;
	UIVertex* V1;
	UIVertex* V2;
	UIVertex* V3;

	//Create Top Face
	
	int32 numVertices = face->mNumVertices;

	if (numVertices == 3)
	{
		V0 = CVertices[3];
		V1 = CVertices[4];
		V2 = CVertices[5];
		UAssimpMeshLibrary::Triangles(mesh, { V0, V1, V2 });
	}
	else if (numVertices == 4)
	{
		V0 = CVertices[4];
		V1 = CVertices[5];
		V2 = CVertices[6];
		V3 = CVertices[7];
		UAssimpMeshLibrary::Quads(mesh, { V0, V1, V2, V3 });
	}
	

	for (int32 i = 0; i < numVertices; i++)
	{
		V0 = CVertices[(i) % numVertices];
		V3 = CVertices[((i) % (numVertices)) + numVertices];
		V2 = CVertices[((i + 1) % (numVertices)) + numVertices];
		V1 = CVertices[(i + 1) % numVertices];

		UAssimpMeshLibrary::Quads(mesh, { V0, V1, V2, V3 });
	}

	if (bDestroyFaceWhenFinished)
	{
		face->Destroy();
	}

	if (updateScene)
	{
		UpdateSceneProxy();
	}

	return;
}

void UEditableMeshComponent::ExtrudeFaces(float Amount)
{
	for (UIFace* face : UEditableMeshLibrary::GetSelectedFaces())
	{
		ExtrudeFace(face, Amount);

		if(face)
			face->Select(false);
	}

	SpawnGBComponents(true);

	UpdateSceneProxy();
}

void UEditableMeshComponent::MergeVertex(TArray<UIVertex*> vertices, bool updateScene)
{
	if (vertices.Num() > 0)
	{
		TArray<FVector> Positions;
		TArray<UIHalfEdge*> AdjecentEdges;

		UIMesh* mesh = vertices[0]->mConnectedEdges[0]->GetFace()->GetMesh();

		for (int32 i = 0; i < vertices.Num(); i++)
		{
			Positions.Add(vertices[i]->GetPosition());
			AdjecentEdges.Append(vertices[i]->mConnectedEdges);
			vertices[i]->mConnectedEdges.Reset();
			mesh->RemoveVertex(vertices[i]);
			vertices[i]->Destroy();
		}

		FVector AverageLocation = UKismetMathLibrary::GetVectorArrayAverage(Positions);

		for (UIHalfEdge* edge : AdjecentEdges)
		{
			edge->SetVertex(UIVertex::CreateVertex(mesh, AverageLocation));
		}

		mesh->CreateEdgeTwins();

		if (updateScene)
		{
			UpdateSceneProxy();
		}
	}
}

void UEditableMeshComponent::MergeVertices()
{
	if (UEditableMeshLibrary::GetSelectedVertices().Num() > 1)
	{
		UIVertex* V0 = UEditableMeshLibrary::GetSelectedVertices()[0];
		UIVertex* V1 = UEditableMeshLibrary::GetSelectedVertices()[1];

		MergeVertex(UEditableMeshLibrary::GetSelectedVertices());

		SpawnGBComponents(true);

		UpdateSceneProxy();

		return;
	}
}

void UEditableMeshComponent::MakeFace()
{
	TArray<UIVertex*> Vertices = UEditableMeshLibrary::GetSelectedVertices();

	if (Vertices.Num() > 0)
	{
		if (Vertices.Num() > 4)
		{
			UAssimpMeshLibrary::TriangleFan(mCurrentMesh, Vertices, Vertices.Num());

			UpdateSceneProxy();
		}
		else if (Vertices.Num() == 4)
		{
			UAssimpMeshLibrary::Quads(mCurrentMesh, Vertices);

			UpdateSceneProxy();
		}
		else if (Vertices.Num() == 3)
		{
			UAssimpMeshLibrary::Triangles(mCurrentMesh, Vertices);

			UpdateSceneProxy();
		}
	}
}

void UEditableMeshComponent::MakeVertex(FVector location)
{
	UIVertex* vertex = UIVertex::CreateVertex(mCurrentMesh, location);

	if (CurrentEditMode == EEditMode::VERTEX)
	{
		CreateGBComponent(vertex, location);
	}
}

void UEditableMeshComponent::DeleteVertex()
{
	for (UIVertex* Vertex : UEditableMeshLibrary::GetSelectedVertices())
	{
		Vertex->Destroy();
	}
}

void UEditableMeshComponent::MoveSelectedElements(UGrabbableBoxComponent* component , FVector location)
{
	FVector Difference = location - component->meshElement->GetPosition();

	if (CurrentEditMode == EEditMode::VERTEX)
	{	
		for (UIVertex* vertex : UEditableMeshLibrary::GetSelectedVertices())
		{
			if (vertex != component->meshElement)
			{
				vertex->SetPosition(vertex->GetPosition() + Difference);
			}
		}
	}

	if (CurrentEditMode == EEditMode::EDGE)
	{
		for (UIHalfEdge* edge : UEditableMeshLibrary::GetSelectedEdges())
		{
			if (edge != component->meshElement)
			{
				edge->SetPosition(edge->GetPosition() + Difference);
			}
		}
	}

	if (CurrentEditMode == EEditMode::FACE)
	{
		for (UIFace* face : UEditableMeshLibrary::GetSelectedFaces())
		{
			if (face != component->meshElement)
			{
				face->SetPosition(face->GetPosition() + Difference);
			}
		}
	}
}

void UEditableMeshComponent::ApplyFalloff(UGrabbableBoxComponent* component, FVector location)
{
	FVector Difference = location - component->meshElement->GetPosition();

	for (UGrabbableBoxComponent* Other : GrabbableBoxComponents)
	{
		if (Other != component && CurrentFallOff != EFallOff::NONE)
		{
			float Amount = 0.0f;

			if (CurrentFallOff == EFallOff::LINEAR)
			{
				Amount = 1.0f / FVector::Distance(component->meshElement->GetPosition(), Other->meshElement->GetPosition());
			}
			else if (CurrentFallOff == EFallOff::CONSTANT)
			{
				Amount = 1.0f;
			}
			else if (CurrentFallOff == EFallOff::QUADRATIC)
			{
				Amount = 1.0f / FVector::DistSquared(component->meshElement->GetPosition(), Other->meshElement->GetPosition());
			}

			Other->meshElement->SetPosition(Other->meshElement->GetPosition() + (Difference * Amount * FallOffStrength));
		}
	}
}

void UEditableMeshComponent::UpdateSceneProxy()
{
	Super::UpdateSceneProxy();

	UpdateComponentLocations();
}
