// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BushPlayerController.generated.h"

class ABushCharacter;
class ABushVolume;

/**
 * Player controller with bush visibility system support
 */
UCLASS()
class STEALTHBUSH_API ABushPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    // Called when this controller is replicating
    virtual void OnRep_Pawn() override; // Useful for handling Pawn assignment after replication
    
    // --- Server RPCs (Client -> Server) ---
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ReportEnteredBush(ABushVolume* Bush);
    bool Server_ReportEnteredBush_Validate(ABushVolume* Bush);
    void Server_ReportEnteredBush_Implementation(ABushVolume* Bush);
    
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ReportExitedBush(ABushVolume* Bush);
    bool Server_ReportExitedBush_Validate(ABushVolume* Bush);
    void Server_ReportExitedBush_Implementation(ABushVolume* Bush);

    // --- Client RPCs (Server -> Client) ---
    UFUNCTION(Client, Reliable)
    void Client_UpdateCharacterVisibility(ABushCharacter* TargetCharacter, bool bShouldBeHidden);
    void Client_UpdateCharacterVisibility_Implementation(ABushCharacter* TargetCharacter, bool bShouldBeHidden);
};