#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshElement.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetPostionSigniture, FVector, NewPosition);

UCLASS(Abstract)
class ASSIMP_API UIObject : public UObject
{
	GENERATED_BODY()

public:

	UIObject();

	virtual ~UIObject();

	UFUNCTION(BlueprintCallable, Category = "MeshObject")
		virtual void Select(bool select);

	UFUNCTION(BlueprintCallable, Category = "MeshObject")
		virtual void Destroy();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MeshObject")
		bool IsElementSelected();

private:
	UPROPERTY()
		bool mSelected;
};

UCLASS(BlueprintType)
class ASSIMP_API UIElement : public UIObject
{

	GENERATED_BODY()

public:

	UIElement();

	virtual ~UIElement();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MeshElement")
		 virtual FORCEINLINE FVector GetPosition();

	UFUNCTION(BlueprintCallable, Category = "MeshElement")
		 virtual void SetPosition(FVector Position);

	UFUNCTION(BlueprintCallable, Category = "MeshElement")
		virtual void OnGrabbed();

	UFUNCTION(BlueprintCallable, Category = "MeshElement")
		virtual void OnReleased();

	UFUNCTION(BlueprintCallable, Category = "MeshElement")
		virtual void OnTriggerPressed();

	UFUNCTION(BlueprintCallable, Category = "MeshElement")
		virtual void OnTriggerReleased();

	virtual void Destroy() override;

	UFUNCTION(BlueprintCallable, Category = "MeshElement")
		void SetGrabbableBoxComponent(UActorComponent* GrabbableBoxComponent);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MeshElement")
		UActorComponent* mGrabbableBoxComponent;

	UPROPERTY(BlueprintAssignable)
		FSetPostionSigniture OnPositionSet;
};