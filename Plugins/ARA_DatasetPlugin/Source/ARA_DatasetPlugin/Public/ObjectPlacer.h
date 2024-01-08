// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ObjectPose.h"

#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPlacer.generated.h"

/*
Class for search Objects by keyword 
And placement it at scene by vector of coordinates
*/

UCLASS()
class ARA_DATASETPLUGIN_API AObjectPlacer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectPlacer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Used by the separate threads to print to console on the main thread. */
	static void PrintToConsole(FString Str, bool Error);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BluePrintCallable, Category = "Object Placer")
		void PlaceObjects(TArray<FObjectPose> ObjectPoses);

private:
	void FindAllInterestedActors(FString ObjectsKeyWord);

public:	
	/*Keyword for search objects at scene*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Placer")
		FString KeyWordInObjectsName = TEXT("TARGET");

private:
	TMap<int32, AActor*> interestedActors;
};
