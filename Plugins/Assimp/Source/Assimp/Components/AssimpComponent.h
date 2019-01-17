// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Components/MeshComponent.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "DynamicMeshBuilder.h"
#include "StaticMeshResources.h"
#include "Engine/Engine.h"
#include "Assimp/Public/Mesh.h"
#include "AssimpComponent.generated.h"



UENUM(BlueprintType)
enum class EShape : uint8
{
	Custom, Plane,Cube,Sphere,Disk,Cone,Cylinder,Pyramid,Pipe,Torus,Octohedren,Tetrahedren
};

UENUM(BlueprintType)
enum class EExportFormat : uint8 
{
	collada, obj, stl, ply
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClearedSigniture);

UCLASS( ClassGroup=(Mesh) , EditInlineNew, meta=(BlueprintSpawnableComponent) )
class ASSIMP_API UAssimpMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAssimpMeshComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*Begin Primitive Component */
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual int32 GetNumMaterials() const override;
	/*End Primitive Component*/

	void ProcessNode(aiNode * node, const aiScene* scene);
	UIMesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
	const aiScene* CreateAssimpScene();

	UFUNCTION(BlueprintCallable)
		void AddMesh(UIMesh*& mesh);

	UFUNCTION(BlueprintCallable)
		void ToggleWireframeMode(bool show);

	UFUNCTION(BlueprintCallable)
		void ToggleCheckerMap( bool show);

	UFUNCTION(BlueprintCallable)
		void ToggleSmoothNormals(bool smooth);

	UFUNCTION(BlueprintCallable)
		void ToggleDrawNormals(bool faces, bool vertices);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool DisplaySmoothNormals();

	UFUNCTION(BlueprintCallable)
		virtual void UpdateSceneProxy();

	UFUNCTION(BlueprintCallable)
		void ConvertMeshToDynamicVertices();

	TArray<uint32> GetIndices() const;

	TArray<FDynamicMeshVertex> GetVertices() const;

	/*Display Mesh Elements on the draw interface*/
	virtual void DrawMeshComponents(FPrimitiveDrawInterface* PDI);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<UIMesh*> mMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UIMesh* mCurrentMesh;

	UFUNCTION(BlueprintCallable)
		bool SelectMesh(int32 index);

	UPROPERTY()
		FString directory;

	UPROPERTY()
		bool bDrawFaceNormals;

	UPROPERTY()
		bool bDrawFaces;

	UPROPERTY()
		bool bDrawVertexNormals;

	UPROPERTY()
		bool bDrawVertices;

	UPROPERTY()
		bool bDrawEdges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float NormalDisplaySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		FColor ElementColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		FColor SelectedElementColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float EdgeThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float PointSize;

	/*Set the material to show checker material*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool bCheckerMap;

	/*Show the mesh's wire frame?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool bWireframe;

	/*The material to use when displaying wire frame, will use engine default wire frame if not set*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		UMaterialInterface* WireframeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		UMaterialInterface* CheckerMapMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		UMaterialInterface* DefaultMaterial;

	/*Import a mesh 
	@PARAM Path - the path to import the mesh from ex: C:\Desktop\mesh.obj
	@PARAM ErrorCode - returns the error if one occurs otherwise is ""
	import formats can be found at: 
	http://assimp.sourceforge.net/main_features_formats.html
	*/
	UFUNCTION(BlueprintCallable)
		bool ImportModel(FString path, FString& ErrorCode);

	/*Export a mesh that was created with this component
	@PARAM Name - is the name of the mesh plus the extension ex. mesh.obj
	@PARAM Path - the path to export the mesh to
	@PARAM Format - the file format to write to NOTE: may not include all export types
	@PARAM SupportedFormats  - returns the current supported export formats
	@PARAM ErrorCode - returns the error if one occurs otherwise is ""
	Export formats .collada, .obj, .stl, .ply should be include in the name, more formats can be found at:
	http://assimp.sourceforge.net/main_features_export.html
	*/
	UFUNCTION(BlueprintCallable)
		bool ExportModel(FString Name, FString Path, FString& SupportedFormats, FString& ErrorCode);


	bool GetSection(int32 Index, TArray<UIVertex*>& Vertices, TArray<UIFace*>& Faces);

	UFUNCTION(BlueprintCallable)
		void Clear();

	UPROPERTY(EditAnywhere, BlueprintAssignable)
		FOnClearedSigniture OnCleared;

private:
	void SetUpBounds();

	UPROPERTY()
		bool bSmoothNormals;

protected:

	int32 _currentMeshProcessed;

	friend class FAssimpMeshSceneProxy;

	TArray<FDynamicMeshVertex> DynamicVertices;

	TArray<uint32> Indices;

	TArray<FVector> BoundPositions;
};


class FAssimpMeshSceneProxy  final : public FPrimitiveSceneProxy
{
public:
	FAssimpMeshSceneProxy(UAssimpMeshComponent* Component)
		:FPrimitiveSceneProxy(Component), VertexFactory(GetScene().GetFeatureLevel(), "FAssimpMeshSceneProxy"),
		MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		AssimpMeshComponent = Component;

		DynamicVertices			= AssimpMeshComponent->GetVertices();
		IndexBuffer.Indices		= AssimpMeshComponent->GetIndices();

		VertexBuffers.InitFromDynamicVertex(&VertexFactory, DynamicVertices);

		if (DynamicVertices.Num() > 0 && IndexBuffer.Indices.Num() > 0)
		{
			//Init render sources
			BeginInitResource(&VertexBuffers.PositionVertexBuffer);
			BeginInitResource(&VertexBuffers.StaticMeshVertexBuffer);
			BeginInitResource(&VertexBuffers.ColorVertexBuffer);
			BeginInitResource(&IndexBuffer);
			BeginInitResource(&VertexFactory);
		}

		Material = AssimpMeshComponent->GetMaterial(0);

		if (Material == NULL)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
	}

	virtual ~FAssimpMeshSceneProxy()
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	};

	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	};

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return (sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return FPrimitiveSceneProxy::GetAllocatedSize();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView *>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_MCMeshComponentSceneProxy_GetDynamicMeshElements);

		//Set when wire frame mode here
		const bool bWireframe = AssimpMeshComponent->bWireframe || (AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe);

		//Get the engine wire frame material and set to color when selected
		FColoredMaterialRenderProxy* WireframeMaterialInstance = NULL;

		if (AssimpMeshComponent->WireframeMaterial)
		{
			WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				AssimpMeshComponent->WireframeMaterial ? AssimpMeshComponent->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
				FLinearColor(0, .5f, 1.f));
		}
		else
		{
			WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
				FLinearColor(0, .5f, 1.f));
		}

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		FMaterialRenderProxy* MaterialProxy = bWireframe ? WireframeMaterialInstance : Material->GetRenderProxy(IsSelected());

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];

				//Draw the mesh
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;
				BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = EPrimitiveType::PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);

				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

				AssimpMeshComponent->DrawMeshComponents(PDI);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance ViewResult;
		ViewResult.bDrawRelevance = IsShown(View);
		ViewResult.bShadowRelevance = IsShadowCast(View);
		ViewResult.bDynamicRelevance = true;
		ViewResult.bRenderInMainPass = ShouldRenderInMainPass();
		ViewResult.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
		ViewResult.bRenderCustomDepth = ShouldRenderCustomDepth();
		MaterialRelevance.SetPrimitiveViewRelevance(ViewResult);
		return ViewResult;
	};

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	};

	/*virtual HHitProxy * CreateHitProxies(UPrimitiveComponent * Component, TArray < TRefCountPtr < HHitProxy > > & OutHitProxies) override
	{
		return new HHitProxy();
	}*/

	UAssimpMeshComponent* AssimpMeshComponent;

	FLocalVertexFactory VertexFactory;

	FMaterialRelevance MaterialRelevance;

	UMaterialInterface* Material;

	FStaticMeshVertexBuffers VertexBuffers;

	FDynamicMeshIndexBuffer32 IndexBuffer;

	TArray<FDynamicMeshVertex> DynamicVertices;
};