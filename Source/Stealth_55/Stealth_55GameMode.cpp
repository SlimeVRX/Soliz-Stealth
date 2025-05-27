// Copyright Epic Games, Inc. All Rights Reserved.

#include "Stealth_55GameMode.h"
#include "Stealth_55Character.h"
#include "UObject/ConstructorHelpers.h"

AStealth_55GameMode::AStealth_55GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
