// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/BushPlayerController.h"
#include "Character/BushCharacter.h"

void ABushPlayerController::Client_UpdateCharacterVisibility_Implementation(
    ABushCharacter* TargetCharacter,
    bool bShouldHide)
{
    if (!TargetCharacter) return;
    
    // Log only when visibility state changes to avoid spam
    bool bCurrentlyHidden = TargetCharacter->IsHidden();
    if (bCurrentlyHidden != bShouldHide)
    {
        UE_LOG(LogTemp, Log, TEXT("CLIENT %s: Updating visibility for %s to %s"), 
               *GetName(), *TargetCharacter->GetName(), bShouldHide ? TEXT("HIDDEN") : TEXT("VISIBLE"));
    }
    
    // Update visibility state
    TargetCharacter->SetActorHiddenInGame(bShouldHide);
}