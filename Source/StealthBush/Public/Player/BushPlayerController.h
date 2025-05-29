// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BushPlayerController.generated.h"

class ABushCharacter;

/**
 * Player controller with bush visibility system support
 */
UCLASS()
class STEALTHBUSH_API ABushPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // Client RPC - Server sends visibility commands
    UFUNCTION(Client, Reliable)
    void Client_UpdateCharacterVisibility(ABushCharacter* TargetCharacter, bool bShouldHide);
    
    // Server RPC - Client reports bush entry/exit
    UFUNCTION(Server, Reliable)
    void Server_ReportEnteredBush(ABushVolume* Bush);
    
    UFUNCTION(Server, Reliable)
    void Server_ReportExitedBush(ABushVolume* Bush);
};