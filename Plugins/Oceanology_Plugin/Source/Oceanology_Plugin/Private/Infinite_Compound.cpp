// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*=================================================
* FileName: Infinite_Compound.cpp
*
* Created by: Galidar
* Project name: Oceanology
* Created on: 2020/18/10
*
* =================================================*/
// Fill out your copyright notice in the Description page of Project Settings.

#include "Infinite_Compound.h"
#include "Kismet/GameplayStatics.h"
#include <Engine/World.h>

// Sets default values for this component's properties
UInfinite_Compound::UInfinite_Compound(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;

	//Defaults
	RealTime = true;
	TimeJump = 10000;

	Origin = GetWorld();
}


// Called when the game starts
void UInfinite_Compound::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Origin = GetWorld();
}


// Called every frame
void UInfinite_Compound::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!IsActive() || !GetAttachParent() || !Origin) return;

	FVector CamLoc;
	FRotator CamRot;
	FVector PawnLoc;
	FVector NewLoc;

#if WITH_EDITOR
	if (GIsEditor)
	{
		if (!RealTime) return;

		TArray<FVector> viewLocations = GetWorld()->ViewLocationsRenderedLastFrame;
		if (viewLocations.Num() != 0)
		{
			CamLoc = viewLocations[0];
		}

		if (InfinityCategory == EInfinityCategory::InfiniteOcean)
		{
			NewLoc = CamLoc;
			NewLoc = NewLoc.GridSnap(TimeJump);
			NewLoc.Z = GetAttachParent()->GetComponentLocation().Z;
			GetAttachParent()->SetWorldLocation(NewLoc);
		}
		else
		{
			GetAttachParent()->SetRelativeLocation(FVector(0, 0, 0));
		}

		float Distance = FMath::Abs(CamLoc.Z - GetAttachParent()->GetComponentLocation().Z);

		return;
	}
#endif

	if (Origin->WorldType == EWorldType::Game || Origin->WorldType == EWorldType::PIE)
	{
		if (!UGameplayStatics::GetPlayerController(Origin, 0)) return;
		if (!UGameplayStatics::GetPlayerController(Origin, 0)->PlayerCameraManager) return;
		UGameplayStatics::GetPlayerController(Origin, 0)->PlayerCameraManager->GetCameraViewPoint(CamLoc, CamRot);

		if (UGameplayStatics::GetPlayerPawn(Origin, 0))
		{
			PawnLoc = UGameplayStatics::GetPlayerPawn(Origin, 0)->GetActorLocation();
		}
		else
		{
			PawnLoc = GetAttachParent()->GetComponentLocation();
		}
	}

	switch (InfinityCategory)
	{
	case EInfinityCategory::InfiniteOcean:
		NewLoc = CamLoc;
		break;
	default:
		break;
	};

	float Distance = FMath::Abs(CamLoc.Z - GetAttachParent()->GetComponentLocation().Z);

	NewLoc = NewLoc.GridSnap(TimeJump);
	NewLoc.Z = GetAttachParent()->GetComponentLocation().Z;
	GetAttachParent()->SetWorldLocation(NewLoc);
}