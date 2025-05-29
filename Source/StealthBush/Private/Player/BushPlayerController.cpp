// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/BushPlayerController.h"
#include "Character/BushCharacter.h"
#include "GameMode/BushGameMode.h"

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

void ABushPlayerController::Server_ReportEnteredBush_Implementation(ABushVolume* Bush)
{
    if (!Bush) return;
    
    // Lấy Character
    ABushCharacter* BushChar = Cast<ABushCharacter>(GetPawn());
    if (!BushChar) return;
    
    // Thông báo cho GameMode
    ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>();
    if (GameMode)
    {
        GameMode->HandlePlayerEnteredBush(BushChar, Bush);
    }
}

void ABushPlayerController::Server_ReportExitedBush_Implementation(ABushVolume* Bush)
{
    if (!Bush) return;
    
    // Lấy Character
    ABushCharacter* BushChar = Cast<ABushCharacter>(GetPawn());
    if (!BushChar) return;
    
    // Thông báo cho GameMode
    ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>();
    if (GameMode)
    {
        GameMode->HandlePlayerExitedBush(BushChar, Bush);
    }
}
