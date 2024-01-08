// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*=================================================
* FileName: QuadTree.cpp
*
* Created by: Galidar
* Project name: Oceanology
* Created on: 2020/31/03
*
* =================================================*/
// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTree.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Modules/ModuleManager.h"

// Sets default values
AQuadTree::AQuadTree(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
	DefaultSceneRoot->CreationMethod = EComponentCreationMethod::Native;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
	
}

void AQuadTree::Tick(float DeltaSeconds)
{
	this->EditorTick(DeltaSeconds);
	//UE_LOG(LogTemp, Warning, TEXT("Actor Tick was called"));
}

void AQuadTree::EditorTick_Implementation(float DeltaSeconds)
{

}

bool AQuadTree::ShouldTickIfViewportsOnly() const
{
	return EditorTickIsEnabled;
}

void AQuadTree::Procedural(UHierarchicalInstancedStaticMeshComponent* HISMC, float GridSize, int GridTiles, float CellSize)
{
	HISMC->ClearInstances();

	const float CellSizeF = GridSize / (float)GridTiles;
	const float HalfCellsX = (float)GridTiles / 2.0f;
	const float HalfCellsY = (float)GridTiles / 2.0f;
	const FVector ActorLocation(GetActorLocation());
	const FVector CellSizeVector(CellSizeF / CellSize);
	const FVector HalfVector(0.5f, 0.5f, 0.0f);

	for (int32 yy = 0; yy < GridTiles; ++yy)
	{
		const float PolyCenterY = (float)yy - HalfCellsY;

		for (int32 xx = 0; xx < GridTiles; ++xx)
		{
			const float PolyCenterX = (float)xx - HalfCellsX;

			FVector PolyCenterVector(PolyCenterX, PolyCenterY, 0.f);
			PolyCenterVector *= CellSizeF;
			PolyCenterVector += HalfVector * CellSizeF;
			PolyCenterVector += ActorLocation;

			HISMC->AddInstance(FTransform(FRotator::ZeroRotator, PolyCenterVector, CellSizeVector));
		}
	}
}