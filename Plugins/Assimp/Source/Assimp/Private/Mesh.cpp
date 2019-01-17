#include "Mesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "EditableMeshLibrary.h"


UIFace::UIFace()
	:mSmoothingGroup(0), mColor(255, 255, 255, 255), mFacePosition(FVector()), mFaceTangents(FITangents(FVector())),
	mMesh(0), mEdge(0)
{

}

UIFace::~UIFace()
{

}

UIFace* UIFace::CreateFace(UIMesh* mesh, UIVertex* vertex0, UIVertex* vertex1, UIVertex* vertex2)
{
	UIFace* face = NewObject<UIFace>(mesh);

	UIHalfEdge* edge0 = UIHalfEdge::CreateHalfEdge(vertex0, face);

	UIHalfEdge* edge1 = UIHalfEdge::CreateHalfEdge(vertex1, face);

	UIHalfEdge* edge2 = UIHalfEdge::CreateHalfEdge(vertex2, face);

	edge0->SetNextEdge(edge1);
	edge1->SetNextEdge(edge2);
	edge2->SetNextEdge(edge0);

	face->mEdges.Add(edge0);
	face->mEdges.Add(edge1);
	face->mEdges.Add(edge2);

	face->mEdge = edge0;

	mesh->AddFace(face);
	mesh->AddVertex(vertex0);
	mesh->AddVertex(vertex1);
	mesh->AddVertex(vertex2);

	face->mNumVertices	= 3;

	return face;
}

UIFace* UIFace::CreateFace(UIMesh* mesh, UIVertex* vertex0, UIVertex* vertex1, UIVertex* vertex2, UIVertex* vertex3)
{
	UIFace* face = NewObject<UIFace>(mesh);

	UIHalfEdge* edge0 = UIHalfEdge::CreateHalfEdge(vertex0, face);
	UIHalfEdge* edge1 = UIHalfEdge::CreateHalfEdge(vertex1, face);
	UIHalfEdge* edge2 = UIHalfEdge::CreateHalfEdge(vertex2, face);
	UIHalfEdge* edge3 = UIHalfEdge::CreateHalfEdge(vertex3, face);

	edge0->SetNextEdge(edge1);
	edge1->SetNextEdge(edge2);
	edge2->SetNextEdge(edge3);
	edge3->SetNextEdge(edge0);

	face->mEdges.Add(edge0);
	face->mEdges.Add(edge1);
	face->mEdges.Add(edge2);
	face->mEdges.Add(edge3);

	face->mEdge = edge0;

	mesh->AddFace(face);
	mesh->AddVertex(vertex0);
	mesh->AddVertex(vertex1);
	mesh->AddVertex(vertex2);
	mesh->AddVertex(vertex3);

	face->mNumVertices	= 4;
	
	return face;
}

FVector UIFace::CalculateNormal()
{
	FVector AB, AC, Cross;

	AB = mEdge->GetVertex()->GetPosition() - mEdge->GetNextEdge()->GetVertex()->GetPosition();
	AC = mEdge->GetVertex()->GetPosition() - mEdge->GetNextEdge()->GetNextEdge()->GetVertex()->GetPosition();

	Cross = AB ^ AC;
	Cross.Normalize();

	mFaceTangents.SetNormal(-Cross);
	mFaceTangents.SetTangentAndBiNormal();

	return -Cross;
}

void UIFace::SetSmoothingGroup(uint32 smoothingGroup)
{
	mSmoothingGroup = smoothingGroup;
}

FVector UIFace::GetPosition()
{
	TArray<FVector> Position;

	for (UIHalfEdge* edge : mEdges)
	{
		Position.Add(edge->GetVertex()->GetPosition());
	}

	/*return (mEdge->GetVertex()->GetPosition() + mEdge->GetNextEdge()->GetVertex()->GetPosition() +
		mEdge->GetNextEdge()->GetNextEdge()->GetVertex()->GetPosition()) / 3.0f;*/

	return UKismetMathLibrary::GetVectorArrayAverage(Position);
}

void UIFace::SetPosition(FVector position)
{
	Super::SetPosition(position);

	TArray<FVector> Offsets;

	for (UIHalfEdge* edge : mEdges)
	{
		Offsets.Add(edge->GetVertex()->GetPosition() - GetPosition());
	}

	for (int32 i = 0; i < mEdges.Num(); ++i)
	{
		mEdges[i]->GetVertex()->SetPosition(Offsets[i] + position);
	}

	/*FVector VertexLocation = mEdge->GetVertex()->GetPosition() - GetPosition();
	FVector NextVertexLocation = mEdge->GetNextEdge()->GetVertex()->GetPosition() - GetPosition();
	FVector NextNextVertexLocation = mEdge->GetNextEdge()->GetNextEdge()->GetVertex()->GetPosition() - GetPosition();

	mEdge->GetVertex()->SetPosition(VertexLocation + position);
	mEdge->GetNextEdge()->GetVertex()->SetPosition(NextVertexLocation + position);
	mEdge->GetNextEdge()->GetNextEdge()->GetVertex()->SetPosition(NextNextVertexLocation + position);*/
}

void UIFace::Destroy()
{
	mMesh->RemoveFace(this);

	UIHalfEdge* edge = mEdge;

	do 
	{
		mMesh->mEdges.Remove(edge);
		edge->Destroy();
		mMesh->RemoveVertex(edge->GetVertex());
		edge->GetVertex()->Destroy();

		edge = edge->GetNextEdge();

	} while (edge != mEdge);

	/*mMesh->mEdges.Remove(mEdge);
	mMesh->mEdges.Remove(mEdge->GetNextEdge());
	mMesh->mEdges.Remove(mEdge->GetNextEdge()->GetNextEdge());

	mEdge->Destroy();
	mEdge->GetNextEdge()->Destroy();
	mEdge->GetNextEdge()->GetNextEdge()->Destroy();

	if (!mEdge->GetVertex()->HasConnectedEdges())
		mMesh->mVertices.Remove(mEdge->GetVertex());

	if (!mEdge->GetNextEdge()->GetVertex()->HasConnectedEdges())
		mMesh->mVertices.Remove(mEdge->GetNextEdge()->GetVertex());

	if (!mEdge->GetNextEdge()->GetNextEdge()->GetVertex()->HasConnectedEdges())
		mMesh->mVertices.Remove(mEdge->GetNextEdge()->GetNextEdge()->GetVertex());*/

	if (IsElementSelected())
	{
		UEditableMeshLibrary::RemoveFacesFromSelected({ this });
	}

	Super::Destroy();
}

void UIFace::Select(bool select)
{
	Super::Select(select);

	if (select)
	{
		UEditableMeshLibrary::AddFacesToSelected({ this });
	}
	else
	{
		UEditableMeshLibrary::RemoveFacesFromSelected({ this });
	}
}

UIHalfEdge* UIFace::GetEdge()
{
	return mEdge;
}

void UIFace::SetEdge(UIHalfEdge* edge)
{
	mEdge = edge;
}

UIMesh* UIFace::GetMesh()
{
	return mMesh;
}

void UIFace::SetMesh(UIMesh* mesh)
{
	mMesh = mesh;
}

//-------------------------- End Face ---------------------------------------------------------------->

//-------------------------- Begin Vertex ---------------------------------------------------------------->

UIVertex::UIVertex()
	:mPosition(FVector()), mTangents(FITangents(FVector())), mTexCoord(FVector2D()), mIndex(0)
{

}



UIVertex::~UIVertex()
{

}

UIVertex* UIVertex::CreateVertex(UIMesh* mesh, FVector position, FVector normal /*= FVector(0.f)*/, FVector2D texCoord /*= FVector2D(0.0f, 0.0f)*/)
{
	UIVertex* vertex = NewObject<UIVertex>(mesh);

	vertex->mPosition = position;
	vertex->mTangents.SetNormal(normal);
	vertex->mTangents.SetTangentAndBiNormal();
	vertex->mTexCoord = texCoord;

	mesh->AddVertex(vertex);

	return vertex;
}

UIVertex* UIVertex::CreateVertex(UIMesh* mesh)
{
	UIVertex* vertex = NewObject<UIVertex>(mesh);

	mesh->AddVertex(vertex);

	return vertex;
}

FVector UIVertex::GetPosition()
{
	return mPosition;
}

void UIVertex::SetPosition(FVector position)
{
	Super::SetPosition(position);

	mPosition = position;

	if (mConnectedEdges.Num() > 0)
	{
		mConnectedEdges[0]->GetFace()->CalculateNormal();
	}
}

void UIVertex::Destroy()
{
	if (mConnectedEdges.Num() == 0)
	{
		if (IsElementSelected())
		{
			UEditableMeshLibrary::RemoveVerticesFromSelected({ this });
		}

		Super::Destroy();
	}
}

void UIVertex::Select(bool select)
{
	Super::Select(select);

	if (select)
	{
		UEditableMeshLibrary::AddVerticesToSelected({ this });
	}
	else
	{
		UEditableMeshLibrary::RemoveVerticesFromSelected({ this });
	}
}

bool UIVertex::HasConnectedEdges()
{
	return mConnectedEdges.Num() > 0;
}

bool UIVertex::operator==(const UIVertex& other) const
{
	return mIndex == other.mIndex &&  mPosition == other.mPosition &&
		mTexCoord == other.mTexCoord && mTangents == other.mTangents &&
		mConnectedEdges == other.mConnectedEdges;
}

//-------------------------- End Vertex---------------------------------------------------------------->
//-------------------------- Begin Edge---------------------------------------------------------------->


UIHalfEdge::UIHalfEdge()
	:mNextEdge(0), mTwinEdge(0), mVertex(0), mFace(0)
{

}


UIHalfEdge::~UIHalfEdge()
	
{

}

UIHalfEdge* UIHalfEdge::CreateHalfEdge(UIVertex* vertex, UIFace* face)
{
	UIHalfEdge* edge = NewObject<UIHalfEdge>(face);

	edge->mVertex = vertex;
	edge->mVertex->mConnectedEdges.AddUnique(edge);
	edge->mFace = face;

	return edge;
}


FVector UIHalfEdge::GetPosition()
{
	return (mVertex->mPosition + mNextEdge->mVertex->mPosition) / 2.0f;
}

void UIHalfEdge::SetPosition(FVector position)
{
	Super::SetPosition(position);

	//Get the difference between the edge the vertices
	FVector VertexLocation = mVertex->GetPosition() - GetPosition();
	FVector NextVertexLocation = mNextEdge->mVertex->GetPosition() - GetPosition();

	mVertex->SetPosition(VertexLocation + position);
	mNextEdge->mVertex->SetPosition(NextVertexLocation + position);
}

void UIHalfEdge::Select(bool select)
{
	Super::Select(select);

	if (select)
	{
		UEditableMeshLibrary::AddEdgesToSelected({ this });

	}
	else
	{
		UEditableMeshLibrary::RemoveEdgesFromSelected({ this });
	}
}

FVector UIHalfEdge::GetNormal()
{
	FVector AB = mNextEdge->mVertex->GetPosition() - mVertex->GetPosition();
	FVector AC = mNextEdge->mNextEdge->mVertex->GetPosition() - mVertex->GetPosition();

	FVector c1 = AB ^ FVector::UpVector;
	FVector c2 = AB ^ FVector::RightVector;

	auto Normal = [c1, c2](FVector A)
	{
		return FVector(A.Y, -A.X, A.Z);
	};

	FVector N = Normal(AB);
	N.Normalize();

	float Dot = FVector::DotProduct(N, AC);

	if (Dot > 0)
	{
		N = -N;
	}

	return N;
}

void UIHalfEdge::SetVertex(UIVertex* vertex)
{
	mVertex = vertex;
}

void UIHalfEdge::SetNextEdge(UIHalfEdge* Edge)
{
	mNextEdge = Edge;
}

void UIHalfEdge::SetTwinEdge(UIHalfEdge* Edge)
{
	mTwinEdge = Edge;
}

void UIHalfEdge::SetFace(UIFace* face)
{
	mFace = face;
}

UIVertex* UIHalfEdge::GetVertex()
{
	return mVertex;
}

UIHalfEdge* UIHalfEdge::GetNextEdge() const
{
	return mNextEdge;
}

UIHalfEdge* UIHalfEdge::GetTwinEdge() const
{
	return mTwinEdge;
}

UIFace* UIHalfEdge::GetFace() const
{
	return mFace;
}

void UIHalfEdge::Destroy()
{
	if (IsElementSelected())
	{
		UEditableMeshLibrary::RemoveEdgesFromSelected({ this });
	}

	if (mTwinEdge)
	{
		mTwinEdge->SetTwinEdge(0);
		mTwinEdge = nullptr;
	}

	mVertex->mConnectedEdges.Remove(this);

	Super::Destroy();
}

bool UIHalfEdge::operator==(const UIHalfEdge& edge) const
{
	return mFace == edge.mFace && mNextEdge == edge.mNextEdge &&
		mTwinEdge == edge.mTwinEdge && mVertex == edge.mVertex;
}

//-------------------------- End Edge---------------------------------------------------------------->

//-------------------------- Begin Mesh---------------------------------------------------------------->

UIMesh::UIMesh() 
	:mCurrentVertexIndex(0)
{

}

UIMesh::~UIMesh()
{
	
}

UIMesh* UIMesh::CreateMesh(TArray<UIFace*> faces, UObject* outer)
{
	UIMesh* mesh = NewObject<UIMesh>(outer);

	for (UIFace* face : faces)
	{
		mesh->AddFace(face);
	}

	return mesh;
}


UIMesh* UIMesh::CreateMesh(UObject* outer)
{
	UIMesh* mesh = NewObject<UIMesh>(outer, UIMesh::StaticClass());

	return mesh;
}

void UIMesh::IncrementCurrentIndex(uint32 amount)
{
	mCurrentVertexIndex += amount;
}

void UIMesh::SetCurrentIndex(uint32 amount)
{
	mCurrentVertexIndex = amount;
}

uint32 UIMesh::GetNextAvailableIndex()
{
	if (mFreeIndices.Num() > 0)
	{
		uint32 index = mFreeIndices[0];
		int32 size = mFreeIndices.Num() - 1;
		mFreeIndices.RemoveAt(size);
		return index;
	}
	else
	{
		//Set to next available index
		return mCurrentVertexIndex;
	}
}

void UIMesh::Destroy()
{
	for (UIFace* face : mFaces)
	{
		
		face->Destroy();
	}

	Super::Destroy();
}


void UIMesh::AutoGenerateUvs()
{
	for (int32 i = 0; i < mFaces.Num(); ++i)
	{
		UIHalfEdge* faceEdge = mFaces[i]->GetEdge();

		do
		{
			FVector pos = faceEdge->GetVertex()->mPosition;

			FVector2D genUV = FVector2D(pos.X, pos.Y);

			genUV.Normalize();

			faceEdge->GetVertex()->mTexCoord = genUV;

			faceEdge = faceEdge->GetNextEdge();

		} while (faceEdge != mFaces[i]->GetEdge());
	}
}

void UIMesh::AddFace(UIFace* face)
{
	UIHalfEdge* edge = face->GetEdge();

	do
	{
		mEdges.Add(edge);

		edge = edge->GetNextEdge();

	} while (edge != face->GetEdge());

	/*mEdges.Add(faceEdge);
	mEdges.Add(faceEdge->GetNextEdge());
	mEdges.Add(faceEdge->GetNextEdge()->GetNextEdge());*/

	face->SetMesh(this);

	face->CalculateNormal();

	mFaces.AddUnique(face);
}

void UIMesh::AddVertex(UIVertex* vertex)
{
	if (mVertices.Find(vertex) == INDEX_NONE)
	{
		mVertices.AddUnique(vertex);

		//if (mFreeIndices.Num() > 0)
		//{
		//	vertex->mIndex = mFreeIndices[0];
		//	int32 size = mFreeIndices.Num() - 1;
		//	mFreeIndices.RemoveAt(size);
		//}
		//else
		//{
		//	//Set to next available index
		//	vertex->mIndex = mCurrentVertexIndex;

		//	mCurrentVertexIndex++;
		//}
	}
}

void UIMesh::RemoveVertex(UIVertex* vertex)
{
	if(vertex->mConnectedEdges.Num() == 0)
		mVertices.Remove(vertex);
}

void UIMesh::CalculateAndSetSmoothNormals()
{	
	for (int32 i = 0; i < mVertices.Num(); i++)
	{
		UIVertex* vertex = mVertices[i];

		TArray<FVector> Normals;
		Normals.AddUninitialized(vertex->mConnectedEdges.Num());

		for (int32 j = 0; j < vertex->mConnectedEdges.Num(); j++)
		{
			UIHalfEdge* edge = vertex->mConnectedEdges[j];

			Normals.Add(edge->GetFace()->CalculateNormal());

			vertex->mTangents.SetNormal(UKismetMathLibrary::GetVectorArrayAverage(Normals));
			vertex->mTangents.SetTangentAndBiNormal();
		}
	}
}

void UIMesh::RemoveFace(UIFace* face)
{
	mFaces.Remove(face);

	mFreeIndices.Append(face->mIndices);
}

void UIMesh::CreateEdgeTwins()
{
	for (UIHalfEdge* edge : mEdges)
	{
		if (UIHalfEdge* other = DoesEdgeHasTwin(edge))
		{
			edge->SetTwinEdge(other);
		}			
	}
}

UIHalfEdge* UIMesh::DoesEdgeHasTwin(UIHalfEdge* edge)
{
	for (UIHalfEdge* otherEdge : mEdges)
	{
		if (edge->GetFace() != otherEdge->GetFace() && edge->GetPosition() == otherEdge->GetPosition())
		{
			return otherEdge;
		}
	}

	return nullptr;
}

