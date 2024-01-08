// Fill out your copyright notice in the Description page of Project Settings.


#include "MetasHelper.h"

void UMetasHelper::SaveMetaData
(
	FString FileName,
	FString SubFolder,
	int32 MetaIndex,
	FString MetaString
)
{
	bool bSubFolderMaked = true;

	if (!FPaths::DirectoryExists(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)))
	{
		bSubFolderMaked = IFileManager::Get().MakeDirectory(*(FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder)));
	}

	if (bSubFolderMaked)
	{
		FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder, FileName + ".txt");
						
		FFileHelper::SaveStringToFile
		(
			FString::Printf(TEXT("[%d] %s\n"), MetaIndex, *MetaString),
			*(PathToFile), 
			FFileHelper::EEncodingOptions::AutoDetect, 
			&IFileManager::Get(), 
			FILEWRITE_Append
		);

		UE_LOG(LogTemp, Log, TEXT("METADATA SAVED IN FILE '%s'"), *PathToFile);
	}
}

void UMetasHelper::ClearMetaFile
(
	FString FileName, 
	FString SubFolder
)
{
	FString PathToFile = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder, FileName + ".txt");

	FFileHelper::SaveStringToFile
	(
		FString::Printf(TEXT("")),
		*(PathToFile),
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		FILEWRITE_None
	);
}

void UMetasHelper::ClearDirectory
(
	FString SubFolder
)
{
	FString PathToDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("\\Saved\\"), SubFolder);
	FPlatformFileManager::Get().Get().GetPlatformFile().DeleteDirectoryRecursively(*PathToDir);
	IFileManager::Get().MakeDirectory(*PathToDir);
	
}
