// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "NorwayTrappedGameMode.h"
#include "NorwayTrappedHUD.h"
#include "NorwayTrappedCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANorwayTrappedGameMode::ANorwayTrappedGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANorwayTrappedHUD::StaticClass();
}
