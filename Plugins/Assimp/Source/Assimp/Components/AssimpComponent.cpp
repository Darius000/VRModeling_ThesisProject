// Fill out your copyright notice in the Description page of Project Settings.

#include "AssimpComponent.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/CollisionProfile.h"
#include "ConstructorHelpers.h"
#include "Assimp/Public/AssimpMeshLibrary.h"
#include "Materials/MaterialInstance.h"

// Sets default values for this component's properties
UAssimpMeshComponent::UAssimpMeshComponent()
	:NormalDisplaySize(5.0f), PointSize(20.0f), EdgeThickness(.2f), bDrawFaceNormals(false), bDrawVertexNormals(false), bDrawVertices(false),
	bDrawEdges(false), ElementColor(255, 128, 0, 255), SelectedElementColor(255, 0, 255, 255), WireframeMaterial(0), 
	bWireframe(false), bDrawFaces(false), bCheckerMap(false), bSmoothNormals(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	// ...

	static ConstructorHelpers::FObjectFinder<UMaterial> WireframeResource(TEXT("/Assimp/Materials/M_Wireframe"));

	if (WireframeResource.Succeeded())
	{
		WireframeMaterial = WireframeResource.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> DefaultMaterialResource(TEXT("/Assimp/Materials/MI_Default"));

	if (DefaultMaterialResource.Succeeded())
	{
		DefaultMaterial = DefaultMaterialResource.Object;

		SetMaterial(0, DefaultMaterial);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> CheckerMaterialResource(TEXT("/Assimp/Materials/MI_Checker"));

	if (CheckerMaterialResource.Succeeded())
	{
		CheckerMapMaterial = CheckerMaterialResource.Object;
	}
}

// Called when the game starts
void UAssimpMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UAssimpMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FPrimitiveSceneProxy* UAssimpMeshComponent::CreateSceneProxy()
{
	if (mMeshes.Num() > 0)
	{
		return new FAssimpMeshSceneProxy(this);
	}

	return NULL;
}

FBoxSphereBounds UAssimpMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBox BoundingBox(ForceInit);

	for (FVector pos : BoundPositions)
	{
		BoundingBox += LocalToWorld.TransformPosition(pos);
	}

	FBoxSphereBounds NBounds;
	NBounds.BoxExtent = BoundingBox.GetExtent();
	NBounds.Origin = BoundingBox.GetCenter();
	NBounds.SphereRadius = BoundingBox.GetExtent().Size();

	return NBounds;
}

int32 UAssimpMeshComponent::GetNumMaterials() const
{
	return 1;
}

bool UAssimpMeshComponent::ImportModel(FString path, FString& ErrorCode)
{
	Assimp::Importer importer;
	
	const aiScene* scene = importer.ReadFile(TCHAR_TO_UTF8(*path), 
		//aiProcessPreset_TargetRealtime_Quality
		//aiProcess_FlipWindingOrder
		aiProcess_CalcTangentSpace                             
		|aiProcess_GenSmoothNormals                           
		|aiProcess_JoinIdenticalVertices              
		|aiProcess_ImproveCacheLocality              
		|aiProcess_LimitBoneWeights                          
		|aiProcess_RemoveRedundantMaterials     
		|aiProcess_SplitLargeMeshes                         
		//|aiProcess_Triangulate                                 
		|aiProcess_GenUVCoords                  
		|aiProcess_SortByPType               
		|aiProcess_FindDegenerates             
		|aiProcess_FindInvalidData
	);

	if (!scene)
	{
		ErrorCode = importer.GetErrorString();

		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorCode);

		return false;
	}

	directory = path;

	ProcessNode(scene->mRootNode, scene);

	UpdateSceneProxy();

	SelectMesh(0);

	return true;
}

bool UAssimpMeshComponent::ExportModel(FString Name, FString Path,  FString& SupportedFormats, FString& ErrorCode)
{
	Assimp::Exporter exporter;

	size_t size = exporter.GetExportFormatCount();

	for (size_t s = 0; s < size; s++)
	{
		const aiExportFormatDesc* ExportDescription = exporter.GetExportFormatDescription(s);

		SupportedFormats.Append("Description: ");
		SupportedFormats.Append(ExportDescription->description);
		SupportedFormats.Append(":: Ext:");
		SupportedFormats.Append(ExportDescription->fileExtension);
		SupportedFormats.Append(":: ID:");
		SupportedFormats.Append(ExportDescription->id);
		SupportedFormats.Append("\n");
	}

	FString FullPath = Path + "\\" + Name;
	const char* exportPath = TCHAR_TO_UTF8(*FullPath);
	const char* extension = TCHAR_TO_UTF8(*FPaths::GetExtension(Name));
	//const char* format = TCHAR_TO_UTF8(*(EnumToString(EExportFormat, Format)));

	/*Create scene to export meshes*/
	const aiScene* scene = CreateAssimpScene();

	if (!exporter.Export(scene, extension, exportPath))
	{
		ErrorCode = exporter.GetErrorString();

		return false;
	}

	return true;
}

bool UAssimpMeshComponent::GetSection(int32 Index, TArray<UIVertex*>& Vertices, TArray<UIFace*>& Faces)
{
	if (Index >= _currentMeshProcessed)
	{
		return false;
	}

	Vertices = mMeshes[Index]->mVertices;
	Faces = mMeshes[Index]->mFaces;

	return true;
}

void UAssimpMeshComponent::Clear()
{
	mMeshes.Empty();

	OnCleared.Broadcast();

	_currentMeshProcessed = 0;
}

void UAssimpMeshComponent::SetUpBounds()
{
	for (int32 m = 0; m < mMeshes.Num(); m++)
	{
		for (int32 f = 0; f < mMeshes[m]->mFaces.Num(); f++)
		{
			UIHalfEdge* faceEdge = mMeshes[m]->mFaces[f]->GetEdge();

			do
			{
				FVector pos = faceEdge->GetVertex()->mPosition;

				BoundPositions.Add(pos);

				faceEdge = faceEdge->GetNextEdge();

			} while (faceEdge != mMeshes[m]->mFaces[f]->GetEdge() && faceEdge);
		}
	}
}

void UAssimpMeshComponent::UpdateSceneProxy()
{
	ConvertMeshToDynamicVertices();

	MarkRenderStateDirty();

	UpdateBounds();
}

void UAssimpMeshComponent::ConvertMeshToDynamicVertices()
{
	DynamicVertices.Reset();
	Indices.Reset();

	//uint32 Index = 0;

	for (UIMesh* mesh : mMeshes)
	{
		mesh->SetCurrentIndex(0);

		for (UIFace* face : mesh->mFaces)
		{
			UIHalfEdge* CurrentEdge = face->GetEdge();

			do
			{
				FDynamicMeshVertex dynVertex;
				dynVertex.Position = CurrentEdge->GetVertex()->mPosition;
				dynVertex.TextureCoordinate[0] = CurrentEdge->GetVertex()->mTexCoord;
				dynVertex.Color = CurrentEdge->GetFace()->mColor;
				/*loop through uv channel if want to add more texture coordinates to vertex 8 is max for DynVertex*/
				if (bSmoothNormals)
					dynVertex.SetTangents(CurrentEdge->GetVertex()->mTangents.GetTangent(), CurrentEdge->GetVertex()->mTangents.GetBiNormal(), CurrentEdge->GetVertex()->mTangents.GetNormal());
				else
					dynVertex.SetTangents(CurrentEdge->GetFace()->mFaceTangents.GetTangent(), CurrentEdge->GetFace()->mFaceTangents.GetBiNormal(), CurrentEdge->GetFace()->mFaceTangents.GetNormal());
				int32 addedIndex = DynamicVertices.Add(dynVertex);
				//CurrentEdge->mVertex->AddDynVertex(&DynamicVertices[addedIndex]);
	
				//Indices.Add(Index);
				//Index++;

				CurrentEdge = CurrentEdge->GetNextEdge();

			} while (CurrentEdge != face->GetEdge());

			if (face->mNumVertices == 3)
			{
				face->mIndices = { mesh->GetNextAvailableIndex(), mesh->GetNextAvailableIndex() + 1, mesh->GetNextAvailableIndex() + 2 };
				mesh->IncrementCurrentIndex(3);
			}
			else if (face->mNumVertices == 4)
			{
				face->mIndices = { mesh->GetNextAvailableIndex(), mesh->GetNextAvailableIndex() + 1, mesh->GetNextAvailableIndex() + 2, mesh->GetNextAvailableIndex() + 2, mesh->GetNextAvailableIndex() + 3, mesh->GetNextAvailableIndex() };
				mesh->IncrementCurrentIndex(4);
			}

			Indices.Append(face->mIndices);
		}

		/*for (int32 f = 0; f < meshes[m].faces.Num(); f++)
		{
			for (int32 i = 0; i < (int32)meshes[m].faces[f].NumVertices; i++)
			{
				//Add indices here
				Indices.Add(meshes[m].faces[f].Indices[i]);
			}
		}*/
	}
}

TArray<uint32> UAssimpMeshComponent::GetIndices() const
{
	return Indices;
}

TArray<FDynamicMeshVertex> UAssimpMeshComponent::GetVertices() const
{
	return DynamicVertices;
}

void UAssimpMeshComponent::DrawMeshComponents(FPrimitiveDrawInterface* PDI)
{
	for (UIMesh* mesh : mMeshes)
	{	
		for (UIFace* face : mesh->mFaces)
		{

			if (bDrawFaceNormals)
			{
				FVector Location = UKismetMathLibrary::TransformLocation(GetComponentTransform(), face->GetPosition());
				FVector NDirection = UKismetMathLibrary::TransformDirection(GetComponentTransform(), face->mFaceTangents.GetNormal());
				FVector BiDirection = UKismetMathLibrary::TransformDirection(GetComponentTransform(), face->mFaceTangents.GetBiNormal());
				FVector TanDirection = UKismetMathLibrary::TransformDirection(GetComponentTransform(), face->mFaceTangents.GetTangent());
				PDI->DrawLine(Location, Location + (NDirection * NormalDisplaySize), FLinearColor::Green, 0);
				PDI->DrawLine(Location, Location + (BiDirection * NormalDisplaySize), FLinearColor::Blue, 0);
				PDI->DrawLine(Location, Location + (TanDirection * NormalDisplaySize), FLinearColor::Red, 0);
					
			}

			if (bDrawFaces)
			{
				FVector Location = UKismetMathLibrary::TransformLocation(GetComponentTransform(), face->GetPosition());
				PDI->DrawPoint(Location, face->IsElementSelected() ?  SelectedElementColor : ElementColor, PointSize, 0);
			}

			UIHalfEdge* edge = face->GetEdge();

			do 
			{
				if (bDrawVertexNormals)
				{
					FVector Location = UKismetMathLibrary::TransformLocation(GetComponentTransform(), edge->GetVertex()->GetPosition());
					FVector NDirection = UKismetMathLibrary::TransformDirection(GetComponentTransform(), edge->GetVertex()->mTangents.GetNormal());
					FVector BiDirection = UKismetMathLibrary::TransformDirection(GetComponentTransform(), edge->GetVertex()->mTangents.GetBiNormal());
					FVector TanDirection = UKismetMathLibrary::TransformDirection(GetComponentTransform(), edge->GetVertex()->mTangents.GetTangent());
					PDI->DrawLine(Location, Location + (NDirection * NormalDisplaySize), FLinearColor::Green, 0);
					PDI->DrawLine(Location, Location + (BiDirection * NormalDisplaySize), FLinearColor::Blue, 0);
					PDI->DrawLine(Location, Location + (TanDirection * NormalDisplaySize), FLinearColor::Red, 0);
				}

				if (bDrawEdges)
				{
					FVector Location = UKismetMathLibrary::TransformLocation(GetComponentTransform(), edge->GetVertex()->GetPosition());
					FVector NextLocation = UKismetMathLibrary::TransformLocation(GetComponentTransform(), edge->GetNextEdge()->GetVertex()->GetPosition());

					//PDI->DrawLine(Location, NextLocation, edge->mTwinEdge ? FLinearColor::Green : FLinearColor::Red, 0, EdgeThickness);
					PDI->DrawLine(Location, NextLocation, edge->IsElementSelected() ? SelectedElementColor : ElementColor, 0, EdgeThickness);
				}

				edge = edge->GetNextEdge();

			} while (edge != face->GetEdge());
		}

		for (UIVertex* vertex : mesh->mVertices)
		{

			if (bDrawVertices)
			{
				FVector Location = UKismetMathLibrary::TransformLocation(GetComponentTransform(), vertex->GetPosition());
				PDI->DrawPoint(Location, vertex->IsElementSelected() ? SelectedElementColor : ElementColor, PointSize, 0);
			}
		}
	}
}

bool UAssimpMeshComponent::SelectMesh(int32 index)
{
	if (mMeshes.Num() > 0 && index < mMeshes.Num())
	{
		mCurrentMesh = mMeshes[index]; 

		return true;
	}

	return false;
}

void UAssimpMeshComponent::ProcessNode(aiNode * node, const aiScene* scene)
{
	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		UIMesh* processedMesh = ProcessMesh(mesh, scene);
		AddMesh(processedMesh);
		++_currentMeshProcessed;
	}

	for (uint32 i =0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

UIMesh* UAssimpMeshComponent::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	UIMesh* newMesh = UIMesh::CreateMesh(this);

	TArray<UIVertex*> vertices;

	FString name = mesh->mName.C_Str();

	//vertices
	for (uint32 i = 0; i < mesh->mNumVertices; i++)
	{
		UIVertex* vertex = UIVertex::CreateVertex(newMesh);

		FVector Vec;
		Vec.X = mesh->mVertices[i].x;
		Vec.Y = mesh->mVertices[i].y;
		Vec.Z = mesh->mVertices[i].z;
		vertex->mPosition = Vec;

		if (mesh->HasTangentsAndBitangents())
		{
			Vec.X = mesh->mTangents[i].x;
			Vec.Y = mesh->mTangents[i].y;
			Vec.Z = mesh->mTangents[i].z;
			vertex->mTangents.SetTangent(Vec);

			Vec.X = mesh->mBitangents[i].x;
			Vec.Y = mesh->mBitangents[i].y;
			Vec.Z = mesh->mBitangents[i].z;
			vertex->mTangents.SetBiNormal(Vec);
		}

		if (mesh->HasNormals())
		{
			Vec.X = mesh->mNormals[i].x;
			Vec.Y = mesh->mNormals[i].y;
			Vec.Z = mesh->mNormals[i].z;
			vertex->mTangents.SetNormal(Vec);
		}

		if (mesh->mTextureCoords[0])
		{
			FVector2D Vec2;
			Vec2.X = mesh->mTextureCoords[0][i].x;
			Vec2.Y = mesh->mTextureCoords[0][i].y;
			vertex->mTexCoord = Vec2;
		}
		else
			vertex->mTexCoord = FVector2D(0.0f, 0.0f);

		vertices.Add(vertex);
	}

	//indices
	for (uint32 i = 0; i < mesh->mNumFaces; i++)
	{		 
		aiFace face = mesh->mFaces[i];
		TArray<UIVertex*> faceVertices;

		for (uint32 j = 0; j < face.mNumIndices; j++)
		{		
			vertices[face.mIndices[j]]->mIndex = face.mIndices[j];
			faceVertices.Add(vertices[face.mIndices[j]]);
		}

		if (faceVertices.Num() == 3)
		{
			UAssimpMeshLibrary::Triangles(newMesh, faceVertices);
		}
		else if (faceVertices.Num() == 4)
		{
			UAssimpMeshLibrary::Quads(newMesh, faceVertices);
		}
		else if (faceVertices.Num() > 4)
		{
			UAssimpMeshLibrary::TriangleFan(newMesh, faceVertices, faceVertices.Num());
		}
	}

	return newMesh;
}

const aiScene* UAssimpMeshComponent::CreateAssimpScene()
{
	//create a new scene
	aiScene* scene = new aiScene();

	//create a new root node
	scene->mRootNode = new aiNode();

	//create materials array
	scene->mMaterials = new aiMaterial*[1];
	scene->mNumMaterials = 1;
	scene->mMaterials[0] = new aiMaterial();

	//create array of meshes
	scene->mMeshes = new aiMesh*[mMeshes.Num()];
	scene->mNumMeshes = mMeshes.Num();

	//add meshes to root node
	scene->mRootNode->mMeshes = new unsigned int[1];
	scene->mRootNode->mMeshes[0] = 0;
	scene->mRootNode->mNumMeshes = 1;

	for (int32 m = 0; m < mMeshes.Num(); m++)
	{
		//create a new mesh
		scene->mMeshes[m] = new aiMesh();

		aiMesh* mesh = scene->mMeshes[m];

		//set the mesh name
		//mesh->mName = TCHAR_TO_UTF8(*meshes[m].name);

		//set the meshes material
		mesh->mMaterialIndex = 0;

		//Create the faces for the aiMesh
		mesh->mFaces = new aiFace[mMeshes[m]->mFaces.Num()];
		mesh->mNumFaces = mMeshes[m]->mFaces.Num();

		mesh->mPrimitiveTypes = aiPrimitiveType::aiPrimitiveType_TRIANGLE;

		for (int32 f = 0; f < mMeshes[m]->mFaces.Num(); f++)
		{
			UIHalfEdge* faceEdge = mMeshes[m]->mFaces[f]->GetEdge();

			mesh->mFaces[f].mNumIndices = 3;
			mesh->mFaces[f].mIndices = new unsigned int[3];
			
			mesh->mFaces[f].mIndices[0] = faceEdge->GetVertex()->mIndex;
			mesh->mFaces[f].mIndices[1] = faceEdge->GetNextEdge()->GetVertex()->mIndex;
			mesh->mFaces[f].mIndices[2] = faceEdge->GetNextEdge()->GetNextEdge()->GetVertex()->mIndex;
		}

		//Create the vertices for the aiMesh
		mesh->mVertices				= new aiVector3D[mMeshes[m]->mVertices.Num()];
		mesh->mNumVertices			= mMeshes[m]->mVertices.Num();
		mesh->mNormals				= new aiVector3D[mMeshes[m]->mVertices.Num()];
		mesh->mTangents				= new aiVector3D[mMeshes[m]->mVertices.Num()];
		mesh->mBitangents			= new aiVector3D[mMeshes[m]->mVertices.Num()];
		mesh->mNumUVComponents[0]	= mMeshes[m]->mVertices.Num();
		mesh->mTextureCoords[0]		= new aiVector3D[mMeshes[m]->mVertices.Num()];
		
		//Set the vertices
		for (int32 v = 0; v < mMeshes[m]->mVertices.Num(); v++)
		{
			aiVector3D Position = aiVector3D(mMeshes[m]->mVertices[v]->mPosition.X, mMeshes[m]->mVertices[v]->mPosition.Y, mMeshes[m]->mVertices[v]->mPosition.Z);
			aiVector3D Normal = aiVector3D(mMeshes[m]->mVertices[v]->mTangents.GetNormal().X, mMeshes[m]->mVertices[v]->mTangents.GetNormal().Y, mMeshes[m]->mVertices[v]->mTangents.GetNormal().Z);
			aiVector3D Tangent = aiVector3D(mMeshes[m]->mVertices[v]->mTangents.GetTangent().X, mMeshes[m]->mVertices[v]->mTangents.GetTangent().Y, mMeshes[m]->mVertices[v]->mTangents.GetTangent().Z);
			aiVector3D Bitangent = aiVector3D(mMeshes[m]->mVertices[v]->mTangents.GetBiNormal().X, mMeshes[m]->mVertices[v]->mTangents.GetBiNormal().Y, mMeshes[m]->mVertices[v]->mTangents.GetBiNormal().Z);
			aiVector3D TexCoord = aiVector3D(mMeshes[m]->mVertices[v]->mTexCoord.X, mMeshes[m]->mVertices[v]->mTexCoord.Y, 0.0f);

			mesh->mVertices[v]				= Position;
			mesh->mNormals[v]				= Normal;
			mesh->mTangents[v]				= Tangent;
			mesh->mBitangents[v]			= Bitangent;
			mesh->mTextureCoords[0][v]		= TexCoord;
		}
	}

	return scene;
}

void UAssimpMeshComponent::AddMesh(UIMesh*& mesh)
{
	mMeshes.Add(mesh);

	mesh->CreateEdgeTwins();

	SetUpBounds();
}

void UAssimpMeshComponent::ToggleWireframeMode(bool show)
{
	bWireframe = show;
}

void UAssimpMeshComponent::ToggleCheckerMap(bool show)
{
	bCheckerMap = show;

	bCheckerMap ? SetMaterial(0, CheckerMapMaterial) : SetMaterial(0, DefaultMaterial);

}

void UAssimpMeshComponent::ToggleSmoothNormals(bool smooth)
{
	bSmoothNormals = smooth;

	if (smooth == true)
	{
		for (UIMesh* mesh : mMeshes)
		{
			mesh->CalculateAndSetSmoothNormals();
		}

		UpdateSceneProxy();
	}
}

void UAssimpMeshComponent::ToggleDrawNormals(bool faces, bool vertices)
{
	bDrawFaceNormals		= faces;
	bDrawVertexNormals		= vertices;
}

bool UAssimpMeshComponent::DisplaySmoothNormals()
{
	return bSmoothNormals;
}

