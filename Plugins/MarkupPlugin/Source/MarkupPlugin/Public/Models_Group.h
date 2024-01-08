// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Models_Group.generated.h"

class UStaticMeshComponent;
class UStaticMeshComponent;

UCLASS()
class MARKUPPLUGIN_API AModels_Group : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AModels_Group();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TMap<int32, TSoftObjectPtr<UStaticMesh>> map_Car_Meshes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		uint8 Unique_object_id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int32 Class_id;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* Root_Component;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Static_Mesh;
	

	FORCEINLINE TMap<int32, TSoftObjectPtr<UStaticMesh>> GetMap() const
	{
		return map_Car_Meshes;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
