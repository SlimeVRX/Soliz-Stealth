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
class STEALTH_55_API ABushPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // Client RPC - Server sends visibility commands
    UFUNCTION(Client, Reliable)
    void Client_UpdateCharacterVisibility(ABushCharacter* TargetCharacter, bool bShouldHide);
    
    // Client RPC - Update UI bush status
    UFUNCTION(Client, Reliable)
    void Client_UpdateBushStatus(int32 NewBushID);

protected:
    // Called when the bush status changes on client
    UFUNCTION(BlueprintImplementableEvent, Category = "Bush System")
    void OnBushStatusChanged(int32 NewBushID);
};