// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/BushPlayerController.h"
#include "Character/BushCharacter.h"
#include "GameMode/BushGameMode.h"

void ABushPlayerController::OnRep_Pawn()
{
    Super::OnRep_Pawn();
}

// --- Server RPCs Implementation ---
bool ABushPlayerController::Server_ReportEnteredBush_Validate(ABushVolume* Bush)
{
    // Basic validation: check if the bush pointer is valid and if the character is valid
    // More complex validation could check distance, line of sight etc., but that would be Game-specific logic.
    ABushCharacter* MyCharacter = Cast<ABushCharacter>(GetPawn());
    if (!MyCharacter || !Bush)
    {
        UE_LOG(LogTemp, Error, TEXT("Server_ReportEnteredBush_Validate failed: Character or Bush is invalid."));
        return false;
    }
    // Optional: Check if the character is actually near or overlapping the bush on the server side
    // This adds robustness against forged RPCs. Requires server-side overlap checking or proximity check.
    // For this example, we rely on the client's local overlap report being truthful,
    // and the server GameMode holding the authoritative state.
    return true;
}

void ABushPlayerController::Server_ReportEnteredBush_Implementation(ABushVolume* Bush)
{
    // This code runs on the Server instance of this PlayerController
    if (HasAuthority()) // Extra safety check, Server RPCs only run on server anyway
    {
        if (ABushCharacter* MyCharacter = Cast<ABushCharacter>(GetPawn()))
        {
            UE_LOG(LogTemp, Warning, TEXT("Server received Server_ReportEnteredBush"));
            // Forward the event to the GameMode for authoritative processing
            if (ABushGameMode* GM = GetWorld()->GetAuthGameMode<ABushGameMode>())
            {
                GM->HandlePlayerEnteredBush(MyCharacter, Bush);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Server_ReportEnteredBush_Implementation failed: Could not cast Pawn to ABushCharacter."));
        }
    }
}

/*void ABushPlayerController::Server_ReportEnteredBush_Implementation(ABushVolume* Bush)
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
}*/

bool ABushPlayerController::Server_ReportExitedBush_Validate(ABushVolume* Bush)
{
    // Similar validation as Server_ReportEnteredBush_Validate
    ABushCharacter* MyCharacter = Cast<ABushCharacter>(GetPawn());
    if (!MyCharacter || !Bush)
    {
        UE_LOG(LogTemp, Error, TEXT("Server_ReportExitedBush_Validate failed: Character or Bush is invalid."));
        return false;
    }
    return true;
}

void ABushPlayerController::Server_ReportExitedBush_Implementation(ABushVolume* Bush)
{
    // This code runs on the Server instance of this PlayerController
    if (HasAuthority()) // Extra safety check
    {
        if (ABushCharacter* MyCharacter = Cast<ABushCharacter>(GetPawn()))
        {
            UE_LOG(LogTemp, Warning, TEXT("Server received Server_ReportExitedBush"));
            // Forward the event to the GameMode for authoritative processing
            if (ABushGameMode* GM = GetWorld()->GetAuthGameMode<ABushGameMode>())
            {
                GM->HandlePlayerExitedBush(MyCharacter, Bush);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Server_ReportExitedBush_Implementation failed: Could not cast Pawn to ABushCharacter."));
        }
    }
}

/*void ABushPlayerController::Server_ReportExitedBush_Implementation(ABushVolume* Bush)
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
}*/

/*void ABushPlayerController::Client_UpdateCharacterVisibility_Implementation(
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
}*/

// --- Client RPCs Implementation ---
void ABushPlayerController::Client_UpdateCharacterVisibility_Implementation(ABushCharacter* TargetCharacter, bool bShouldBeHidden)
{
    // This code runs on the Client instance of this PlayerController
    // It should not run on the server (IsNetMode(NM_Client) check is implicit for Client RPCs)
    if (TargetCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Client %s received Client_UpdateCharacterVisibility for character %s, hidden=%s"), *GetName(), *TargetCharacter->GetName(), bShouldBeHidden ? TEXT("true") : TEXT("false"));
        // Call the function on the target character to set its visibility on this specific client
        TargetCharacter->SetCharacterVisibility(bShouldBeHidden);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Client_UpdateCharacterVisibility_Implementation failed: TargetCharacter is invalid."));
    }
}