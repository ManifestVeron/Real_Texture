// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MetasHelper.generated.h"

/**
 *	Класс для построения метаданных. Проект Аврора
 */

UCLASS()
class VIRTUAL_POLYGON_API UMetasHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Metas Helper | Common Method for saving meta")
	static void SaveMetaData
	(
		FString FileName,
		FString SubFolder,
		int32 MetaIndex,
		FString MetaString
	);

	UFUNCTION(BlueprintCallable, Category = "Metas Helper | Common Method for clearing metafile")
	static void ClearMetaFile
	(
		FString FileName,
		FString SubFolder
	);

	UFUNCTION(BlueprintCallable, Category = "Metas Helper | Common Method for clearing directory")
	static void ClearDirectory
	(
		FString SubFolder
	);
};
