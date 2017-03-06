// Fill out your copyright notice in the Description page of Project Settings.

#include "HoverVehicle.h"
#include "HoverVehicleGameModeBase.h"





AHoverVehicleGameModeBase::AHoverVehicleGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/HoverVehicleNew_BP"));
	if(PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
