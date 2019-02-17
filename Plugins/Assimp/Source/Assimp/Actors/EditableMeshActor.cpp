// Fill out your copyright notice in the Description page of Project Settings.

#include "EditableMeshActor.h"
#include "Assimp/Public/EditableMeshLibrary.h"

AEditableMeshActor::AEditableMeshActor()
	:Shape(EShape::Plane)
{
	PrimaryActorTick.bCanEverTick = false;

	EditableMeshComponent = CreateDefaultSubobject<UEditableMeshComponent>(TEXT("EditableMesh"));

	GrabbableComponent = CreateDefaultSubobject<UGrabbableComponent>(TEXT("GrabbableComponent"));

	GrabbableComponent->bHiddenInGame = false;

	SetRootComponent(GrabbableComponent);

	EditableMeshComponent->SetupAttachment(RootComponent);

	GrabbableComponent->SetBoxExtent(FVector(1.0f));
}

void AEditableMeshActor::BeginPlay()
{
	Super::BeginPlay();

	//EditableMeshComponent->OnEditModeChanged.AddDynamic(this, &AEditableMeshActor::UpdateGrabbableComponent);

	GrabbableComponent->OnGrab.AddDynamic(this, &AEditableMeshActor::Grab);

	OnDestroyed.AddDynamic(this, &AEditableMeshActor::RemoveAsCurrentMeshComponent);

	CreateShape(DefaultData);
}

void AEditableMeshActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	RemoveAsCurrentMeshComponent(this);
}

/*void AEditableMeshActor::UpdateGrabbableComponent(EEditMode Mode)
{
	if (Mode == EEditMode::OBJECT)
	{
		GrabbableComponent->SetBoxExtent(EditableMeshComponent->Bounds.BoxExtent);
	}
}*/

void AEditableMeshActor::Grab(USceneComponent* attatchTo, FName socket, EControllerHand hand)
{
	UEditableMeshLibrary::SetCurrentEditableMeshComponent(EditableMeshComponent);
}

void AEditableMeshActor::RemoveAsCurrentMeshComponent(AActor* destroyedActor)
{
	if (UEditableMeshLibrary::GetCurrentEditableMeshComponent() == EditableMeshComponent)
	{
		EditableMeshComponent->SetEditMode(EEditMode::OBJECT);

		UEditableMeshLibrary::SetCurrentEditableMeshComponent(nullptr);

		UEditableMeshLibrary::GetSelectedEdges().Reset();
		UEditableMeshLibrary::GetSelectedFaces().Reset();
		UEditableMeshLibrary::GetSelectedVertices().Reset();
	}
}


void AEditableMeshActor::CreateShape(FData data)
{
	switch (Shape)
	{
	case EShape::Plane:
		EditableMeshComponent->Plane(data.width, data.height);
		break;
	case EShape::Cube:
		EditableMeshComponent->Cube(data.size);
		break;
	case EShape::Sphere:
		EditableMeshComponent->Sphere(data.size, data.divisions);
		break;
	case EShape::Disk:
		EditableMeshComponent->Disk(data.size, data.divisions, 0.0f);
		break;
	case EShape::Cone:
		EditableMeshComponent->Cone(data.size, data.height, data.divisions);
		break;
	case EShape::Cylinder:
		EditableMeshComponent->Cylinder(data.size, data.height, data.divisions);
		break;
	case EShape::Pyramid:
		EditableMeshComponent->Pyramid(data.size, data.height);
		break;
	case EShape::Pipe:
		EditableMeshComponent->Pipe(data.size, data.innerSize, data.height, data.divisions);
		break;
	case EShape::Torus:
		EditableMeshComponent->Torus(data.size, data.innerSize, data.divisions);
		break;
	case EShape::Octohedren:
		EditableMeshComponent->Octohedren(data.size);
		break;
	case EShape::Tetrahedren:
		EditableMeshComponent->Tetrahedron(data.size);
		break;
	default:
		break;
	case EShape::Custom:

		FString errorCode = "";

		EditableMeshComponent->ImportModel(data.importPath, errorCode);
		break;
	}

	//UpdateGrabbableComponent(EEditMode::OBJECT);

	return;
}

