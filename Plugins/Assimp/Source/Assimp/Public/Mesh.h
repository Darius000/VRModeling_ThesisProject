#pragma once


#include "Tangents.h"
#include "DynamicMeshBuilder.h"
#include "MeshElement.h"
#include "Mesh.generated.h"

class UIElement;
class UIHalfEdge;
class UIVertex;
class UIMesh;
class UIFace;



UCLASS(BlueprintType)
class ASSIMP_API UIFace : public UIElement
{
	GENERATED_BODY()

public:
	UIFace();

	virtual ~UIFace();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UIFace* CreateFace(UIMesh* mesh, UIVertex* vertex0, UIVertex* vertex1, UIVertex* vertex2);

	static UIFace* CreateFace(UIMesh* mesh, UIVertex* vertex0, UIVertex* vertex1, UIVertex* vertex2, UIVertex* vertex3);

	FVector CalculateNormal();

	void SetSmoothingGroup(uint32 smoothingGroup);

	virtual FVector GetPosition() override;

	virtual void SetPosition(FVector position) override;

	virtual void Destroy() override;

	virtual void Select(bool select) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector mFacePosition;

	uint32 mSmoothingGroup;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FITangents mFaceTangents;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FColor mColor;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UIHalfEdge* GetEdge();

	UFUNCTION(BlueprintCallable)
		void SetEdge(UIHalfEdge* edge);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UIMesh* GetMesh();

	UFUNCTION(BlueprintCallable)
		void SetMesh(UIMesh* mesh);

	uint32 mNumVertices;

	TArray<uint32> mIndices;

private:
	UPROPERTY(VisibleAnywhere)
		UIHalfEdge* mEdge;

	UPROPERTY(VisibleAnywhere)
		UIMesh* mMesh;

	TArray<UIHalfEdge*> mEdges;
};

UCLASS(BlueprintType)
class ASSIMP_API UIVertex : public UIElement
{
	GENERATED_BODY()

public:

	UIVertex();

	virtual ~UIVertex();

	static UIVertex* CreateVertex(UIMesh* mesh);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UIVertex* CreateVertex(UIMesh* mesh, FVector position, FVector normal = FVector(0.f), FVector2D texCoord = FVector2D(0.0f, 0.0f));

	virtual FVector GetPosition() override;

	virtual void SetPosition(FVector position) override;

	virtual void Destroy() override;

	virtual void Select(bool select) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool HasConnectedEdges();

	//void AddDynVertex(FDynamicMeshVertex* dynVertex);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector mPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D mTexCoord;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FITangents mTangents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UIHalfEdge*> mConnectedEdges;

	uint32 mIndex;

	bool operator==(const UIVertex& other) const;
};

UCLASS(BlueprintType)
class ASSIMP_API UIHalfEdge : public UIElement
{
	GENERATED_BODY()

public:
	UIHalfEdge();

	virtual ~UIHalfEdge();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UIHalfEdge* CreateHalfEdge(UIVertex* vertex, UIFace* face);

	virtual FVector GetPosition() override;

	virtual void SetPosition(FVector position) override;

	virtual void Select(bool select) override;


	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetNormal();

	UFUNCTION(BlueprintCallable)
		void SetVertex(UIVertex* vertex);

	UFUNCTION(BlueprintCallable)
		void SetNextEdge(UIHalfEdge* Edge);

	UFUNCTION(BlueprintCallable)
		void SetTwinEdge(UIHalfEdge* Edge);

	UFUNCTION(BlueprintCallable)
		void SetFace(UIFace* face);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UIVertex* GetVertex();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UIHalfEdge* GetNextEdge() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UIHalfEdge* GetTwinEdge() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UIFace* GetFace() const;

	void Destroy() override;

private:
	UPROPERTY(VisibleAnywhere)
		UIHalfEdge* mNextEdge;

	UPROPERTY(VisibleAnywhere)
		UIHalfEdge* mTwinEdge;

	UPROPERTY(VisibleAnywhere)
		UIFace* mFace;

	UPROPERTY(VisibleAnywhere)
		UIVertex* mVertex;
public:
	bool operator==(const UIHalfEdge& edge) const;
};

UCLASS(BlueprintType)
class ASSIMP_API UIMesh : public UIObject
{
	GENERATED_BODY()

public:
	UIMesh();

	virtual ~UIMesh();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UIMesh* CreateMesh(UObject* outer);

	static UIMesh* CreateMesh(TArray<UIFace*> faces, UObject* outer);

	void IncrementCurrentIndex(uint32 amount);

	void SetCurrentIndex(uint32 amount);

	uint32 GetNextAvailableIndex();

	void Destroy() override;

	UFUNCTION(BlueprintCallable)
		void AutoGenerateUvs();

	UFUNCTION(BlueprintCallable)
		void AddFace(UIFace* face);

	UFUNCTION(BlueprintCallable)
		void RemoveFace(UIFace* face);

	UFUNCTION(BlueprintCallable)
		void AddVertex(UIVertex* vertex);

	UFUNCTION(BlueprintCallable)
		void RemoveVertex(UIVertex* vertex);

	void CalculateAndSetSmoothNormals();

	void CreateEdgeTwins();

	UIHalfEdge* DoesEdgeHasTwin(UIHalfEdge* edge);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UIVertex*> mVertices;

	TArray<uint32> mIndices;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UIFace*> mFaces;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UIHalfEdge*> mEdges;

private:
	uint32 mCurrentVertexIndex;

	TArray<uint32> mFreeIndices;
};