// Fill out your copyright notice in the Description page of Project Settings.

#include "GrabbableBoxComponent.h"
#include "ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"


#define SELECTEDCOLOR FColor(255, 128, 0)
#define OVERLAPPEDCOLOR FColor(255, 0, 0)
#define DEFAULTCOLOR FColor(255, 255, 255)


UGrabbableBoxComponent::UGrabbableBoxComponent()
	:mSelected(false)
{
	PrimaryComponentTick.bCanEverTick = true;

	SetCollisionResponseToChannel(GRABBABLECOMPONENT_COLLISION, ECR_Overlap);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//ShapeColor = FColor(255, 255, 255);

	SphereRadius = .5f;

	/*static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Assimp/Materials/ElementMaterial"));

	if (Material.Succeeded())
	{
		VertexMaterial = Material.Object;
	}*/
}

UGrabbableBoxComponent::~UGrabbableBoxComponent()
{
	
}

void UGrabbableBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	/*OnComponentBeginOverlap.AddDynamic(this, &UGrabbableBoxComponent::BeginOverlapped);

	OnComponentEndOverlap.AddDynamic(this, &UGrabbableBoxComponent::EndOverlapped);*/

	OwnerActor = GetOwner();

	//ShapeColor = DEFAULTCOLOR;

	/*materialBillboardComponent = NewObject<UMaterialBillboardComponent>(this, UMaterialBillboardComponent::StaticClass());
	materialBillboardComponent->SetupAttachment(this);
	materialBillboardComponent->SetWorldScale3D(FVector(0.01f));
	materialBillboardComponent->bHiddenInGame = false;
	materialBillboardComponent->RegisterComponent();*/	

	//if (VertexMaterial)
	//{
	//	VertexDynmicMaterial = UMaterialInstanceDynamic::Create(VertexMaterial, this);
	//	//materialBillboardComponent->AddElement(VertexDynmicMaterial, nullptr, false, 1.0f, 1.0f, nullptr);
	//}

	SetComponentTickEnabled(false);
}

void UGrabbableBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (grabbed && meshElement)
	{
		//use kismet math library to get relative position to mesh owner

		//FVector location = UKismetMathLibrary::TransformLocation(OwnerActor->GetTransform(), GetComponentLocation());
		FVector location = UKismetMathLibrary::ConvertTransformToRelative(OwnerActor->GetTransform(), GetComponentTransform()).GetLocation();	
		OnPositionUpdated.Broadcast(this, location);
		OnUpdated.Broadcast();
		meshElement->SetPosition(location);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGrabbableBoxComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	//if (materialBillboardComponent) materialBillboardComponent->DestroyComponent();
}


FPrimitiveSceneProxy* UGrabbableBoxComponent::CreateSceneProxy()
{
	return nullptr;
}

void UGrabbableBoxComponent::CreateMaterialBillboardComponent()
{
	
}

void UGrabbableBoxComponent::SetMeshElememt(UIElement* element)
{
	meshElement = element;

	if(element && this)
		element->SetGrabbableBoxComponent(this);
}

void UGrabbableBoxComponent::BeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/*if (mSelected == false && OtherComp->ComponentHasTag("UserGrab"))
	{
		ShapeColor = OVERLAPPEDCOLOR;

		MarkRenderStateDirty();
	}*/
}

void UGrabbableBoxComponent::EndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (mSelected == false && OtherComp->ComponentHasTag("UserGrab"))
	{
		ShapeColor = DEFAULTCOLOR;

		MarkRenderStateDirty();
	}*/
}

void UGrabbableBoxComponent::Grab_Implementation(USceneComponent* attatchTo, FName socket, EControllerHand hand)
{
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	rules.ScaleRule = EAttachmentRule::KeepRelative;

	AttachToComponent(attatchTo, rules, socket);

	OnGrab.Broadcast(attatchTo, socket, hand);
	
	if(meshElement)
		meshElement->OnGrabbed();

	grabbed = true;

	/*if (mSelected == false)
	{
		ShapeColor = OVERLAPPEDCOLOR;

		MarkRenderStateDirty();
	}*/

	SetComponentTickEnabled(true);
}

void UGrabbableBoxComponent::Release_Implementation()
{
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);

	AttachToComponent(OwnerActor->GetRootComponent(), rules);

	OnRelease.Broadcast();

	if(meshElement)
		meshElement->OnReleased();

	grabbed = false;

	/*if (mSelected == false)
	{
		ShapeColor = DEFAULTCOLOR;

		MarkRenderStateDirty();
	}*/

	SetComponentTickEnabled(false);
}

void UGrabbableBoxComponent::TriggerPressed_Implementation()
{
	Super::TriggerPressed_Implementation();

	meshElement->OnTriggerPressed();

	Select();
}

void UGrabbableBoxComponent::TriggerReleased_Implementation()
{
	Super::TriggerReleased_Implementation();

	meshElement->OnTriggerReleased();
}

void UGrabbableBoxComponent::CanBeGrabbed_Implementation(bool& IsGrabbableReady)
{
	IsGrabbableReady = canInteractWith && canGrab;
}

void UGrabbableBoxComponent::Select()
{
	mSelected = !mSelected;

	if (meshElement)
		meshElement->Select(mSelected);

	/*mSelected ? ShapeColor = SELECTEDCOLOR : ShapeColor = DEFAULTCOLOR;

	VertexDynmicMaterial->SetVectorParameterValue("Tint", mSelected ? FLinearColor::Red : FLinearColor::White);

	MarkRenderStateDirty();*/
}

bool UGrabbableBoxComponent::Selected()
{
	return mSelected;
}
