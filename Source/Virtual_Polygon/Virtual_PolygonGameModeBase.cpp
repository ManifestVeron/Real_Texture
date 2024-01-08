// Fill out your copyright notice in the Description page of Project Settings.


#include "Virtual_PolygonGameModeBase.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AVirtual_PolygonGameModeBase::AVirtual_PolygonGameModeBase()
{
	PlayerControllerClass = StaticClass();
}
