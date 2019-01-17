// Fill out your copyright notice in the Description page of Project Settings.

#include "FileFunctions.h"
#include "Core/Public/HAL/FileManagerGeneric.h"
#include "Core/Public/Misc/FileHelper.h"
#include "Core/Public/Misc/Paths.h"

bool UFileFunctions::GetFiles(TArray<FString>& Files, TArray<FString>& FilePaths, FString Directory, FString Ext /*= ""*/, bool bFiles /*= true*/, bool bDirectories /*= false*/, bool bRootDirectory /*= true*/)
{
	if (Directory.Len() < 1) return false;

	FPaths::NormalizeDirectoryName(Directory);

	IFileManager& FileManager = IFileManager::Get();

	if (Ext == "")
	{
		Ext = "*.*";
	}
	else
	{
		Ext = (Ext.Left(1) == ".") ? "*" + Ext : "*." + Ext;
	}

	FString RootDirectory = Directory + "/";
	FString FinalPath = RootDirectory + Ext ;
	FileManager.FindFiles(Files, *FinalPath, bFiles, bDirectories);

	if (bRootDirectory)
	{		
		FilePaths.Add(RootDirectory);
		FileManager.FindFiles(Files, *RootDirectory, bFiles, bDirectories);
	}

	for (FString File : Files)
	{
		FilePaths.Add(RootDirectory + File);
	}

	return true;
}


