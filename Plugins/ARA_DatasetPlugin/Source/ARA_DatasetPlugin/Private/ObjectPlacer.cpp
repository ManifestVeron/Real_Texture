// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPlacer.h"

#include "Logging/MessageLog.h"
#include "EngineUtils.h"
#include "Kismet/KismetStringLibrary.h"

/*
* AObjectPlacer
*/

// Sets default values
AObjectPlacer::AObjectPlacer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AObjectPlacer::BeginPlay()
{
	Super::BeginPlay();
	FindAllInterestedActors(KeyWordInObjectsName);
}

// Called every frame
void AObjectPlacer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AObjectPlacer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AObjectPlacer::PrintToConsole(FString Str, bool Error)
{
	if (Error)
	{
		auto messageLog = FMessageLog("ARA Object Placer");
		messageLog.Open(EMessageSeverity::Error, true);
		messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
	}
}


void AObjectPlacer::FindAllInterestedActors(FString ObjectsKeyWord)
{
	FActorIterator AllActorsItr = FActorIterator(GetWorld());

	interestedActors.Empty();

	UE_LOG(LogTemp, Warning, TEXT("!!! LIST OF ALL INTERESTED ACTORS !!!"))

		while (AllActorsItr)
		{
			FString actorLabel(AllActorsItr->GetFName().ToString());

			if (actorLabel.Contains(ObjectsKeyWord))
			{
				FString strObjectNum = UKismetStringLibrary::GetSubstring(actorLabel, 2, 3);

				int32 indexObject = UKismetStringLibrary::Conv_StringToInt(strObjectNum);

				UE_LOG(LogTemp, Log, TEXT("Actor Name: %s | Location: %s | Rotation: %s"),
					*AllActorsItr->GetFName().ToString(),
					*AllActorsItr->GetActorLocation().ToString(),
					*AllActorsItr->GetActorRotation().ToString());

				interestedActors.Add(indexObject, *AllActorsItr);
			}

			++AllActorsItr;
		}

	UE_LOG(LogTemp, Warning, TEXT("COUNT OF INTERESTED ACTORS --> %d"), interestedActors.Num());

}


void AObjectPlacer::PlaceObjects(TArray<FObjectPose> ObjectPoses)
{
	for (auto iter = interestedActors.begin(); iter != interestedActors.end(); ++iter)
	{
		int32 indexObj = (iter->Key);

		if (ObjectPoses.IsValidIndex(indexObj))
		{
			FObjectPose objPose = ObjectPoses[indexObj];
			
			iter->Value->SetActorLocation(FVector(objPose.xc, objPose.yc, objPose.zc), false, (FHitResult*)nullptr, ETeleportType::ResetPhysics); // set position
			iter->Value->SetActorRotation(FRotator(objPose.ry, objPose.rz, objPose.rx), ETeleportType::ResetPhysics); // set rotation Pitch RY Yaw RZ  Roll RX
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("OBJECTS WAS REPLACED"));
}