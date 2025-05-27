// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/BushPlayerState.h"
#include "Player/BushPlayerController.h"
#include "Net/UnrealNetwork.h"

ABushPlayerState::ABushPlayerState()
{
    // Enable replication
    bReplicates = true;
}

void ABushPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // Register replicated properties
    DOREPLIFETIME(ABushPlayerState, CurrentBushID);
    DOREPLIFETIME(ABushPlayerState, FactionID);
}

void ABushPlayerState::SetCurrentBushID(int32 NewBushID)
{
    // Only server can set this
    if (!HasAuthority()) return;
    
    // Only update if changed
    if (CurrentBushID != NewBushID)
    {
        CurrentBushID = NewBushID;
        
        // Notify the owning client about bush status change
        if (ABushPlayerController* PC = Cast<ABushPlayerController>(GetOwningController()))
        {
            PC->Client_UpdateBushStatus(CurrentBushID);
        }
    }
}

void ABushPlayerState::SetFactionID(int32 NewFactionID)
{
    // Only server can set this
    if (!HasAuthority()) return;
    
    // Only update if changed
    if (FactionID != NewFactionID)
    {
        FactionID = NewFactionID;
        
        // If we need to notify about faction changes, add code here
        UE_LOG(LogTemp, Log, TEXT("Player %s faction changed to %d"), 
               *GetPlayerName(), FactionID);
    }
}

void ABushPlayerState::OnRep_CurrentBushID()
{
    // This executes on clients when CurrentBushID is replicated
    
    // Update UI through player controller
    if (ABushPlayerController* PC = Cast<ABushPlayerController>(GetOwningController()))
    {
        PC->Client_UpdateBushStatus(CurrentBushID);
    }
    
    // Log for debugging
    UE_LOG(LogTemp, Log, TEXT("Client: Bush status updated to %d for player %s"), 
           CurrentBushID, *GetPlayerName());
    
    // Could trigger visual effects here based on bush status
    if (CurrentBushID != -1)
    {
        // Player entered a bush - could play sound/effect
    }
    else
    {
        // Player exited a bush - could play sound/effect
    }
}