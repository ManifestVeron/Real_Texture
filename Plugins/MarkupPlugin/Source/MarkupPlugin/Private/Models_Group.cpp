// Fill out your copyright notice in the Description page of Project Settings.


#include "Models_Group.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AModels_Group::AModels_Group()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root_Component = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(Root_Component);
	
	Static_Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	Static_Mesh->SetupAttachment(Root_Component);
	
}

// Called when the game starts or when spawned
void AModels_Group::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AModels_Group::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

