// Fill out your copyright notice in the Description page of Project Settings.

#include "EditableMeshLibrary.h"

UEditableMeshComponent* UEditableMeshLibrary::GetCurrentEditableMeshComponent()
{
	return CurrentEditableMeshComponent;
}

void UEditableMeshLibrary::SetCurrentEditableMeshComponent(UEditableMeshComponent* EditableMeshComponent)
{
	if (CurrentEditableMeshComponent)
		CurrentEditableMeshComponent->SetEditMode(EEditMode::OBJECT);

	CurrentEditableMeshComponent = EditableMeshComponent;
}

TArray<UIVertex*> UEditableMeshLibrary::GetSelectedVertices()
{
	return mSelectedVertices;
}

TArray<UIHalfEdge*> UEditableMeshLibrary::GetSelectedEdges()
{
	return mSelectedEdges;
}

TArray<UIFace*> UEditableMeshLibrary::GetSelectedFaces()
{
	return mSelectedFaces;
}

void UEditableMeshLibrary::AddVerticesToSelected(TArray<UIVertex*> vertices)
{
	for (int32 i = 0; i < vertices.Num(); ++i)
	{
		mSelectedVertices.AddUnique(vertices[i]);
	}
}

void UEditableMeshLibrary::AddEdgesToSelected(TArray<UIHalfEdge*> edges)
{
	for (int32 i = 0; i < edges.Num(); ++i)
	{
		mSelectedEdges.AddUnique(edges[i]);
	}
}

void UEditableMeshLibrary::AddFacesToSelected(TArray<UIFace*> faces)
{
	for (int32 i = 0; i < faces.Num(); ++i)
	{
		mSelectedFaces.AddUnique(faces[i]);
	}
}

void UEditableMeshLibrary::RemoveVerticesFromSelected(TArray<UIVertex*> vertices)
{
	for (int32 i = 0; i < vertices.Num(); ++i)
	{
		mSelectedVertices.Remove(vertices[i]);
	}
}

void UEditableMeshLibrary::RemoveEdgesFromSelected(TArray<UIHalfEdge*> edges)
{
	for (int32 i = 0; i < edges.Num(); ++i)
	{
		mSelectedEdges.Remove(edges[i]);
	}
}

void UEditableMeshLibrary::RemoveFacesFromSelected(TArray<UIFace*> faces)
{
	for (int32 i = 0; i < faces.Num(); ++i)
	{
		mSelectedFaces.Remove(faces[i]);
	}
}


UEditableMeshComponent* UEditableMeshLibrary::CurrentEditableMeshComponent;


TArray<UIFace*> UEditableMeshLibrary::mSelectedFaces;

TArray<UIHalfEdge*> UEditableMeshLibrary::mSelectedEdges;

TArray<UIVertex*> UEditableMeshLibrary::mSelectedVertices;