// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/BushPlayerController.h"
#include "Character/BushCharacter.h"

void ABushPlayerController::Client_UpdateCharacterVisibility_Implementation(
    ABushCharacter* TargetCharacter,
    bool bShouldHide)
{
    if (TargetCharacter)
    {
        TargetCharacter->SetActorHiddenInGame(bShouldHide);
        
        UE_LOG(LogTemp, Log, TEXT("Client: %s character %s"), 
               bShouldHide ? TEXT("Hiding") : TEXT("Showing"), 
               *TargetCharacter->GetName());
    }
}

void ABushPlayerController::Client_UpdateBushStatus_Implementation(int32 NewBushID)
{
    // Call blueprint event for UI updates
    OnBushStatusChanged(NewBushID);
    
    UE_LOG(LogTemp, Log, TEXT("Client: Bush status updated to %d"), NewBushID);
}