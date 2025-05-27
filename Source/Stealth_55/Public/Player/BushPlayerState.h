// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BushPlayerState.generated.h"

/**
 * Player state with bush system data
 */
UCLASS()
class STEALTH_55_API ABushPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ABushPlayerState();
    
    // Setters (Server only)
    void SetCurrentBushID(int32 NewBushID);
    void SetFactionID(int32 NewFactionID);
    
    // Getters
    UFUNCTION(BlueprintPure, Category = "Bush System")
    int32 GetCurrentBushID() const { return CurrentBushID; }
    
    UFUNCTION(BlueprintPure, Category = "Bush System")
    int32 GetFactionID() const { return FactionID; }
    
    // Replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    // Called when CurrentBushID changes on client
    UFUNCTION()
    void OnRep_CurrentBushID();
    
private:
    // Bush system data
    UPROPERTY(ReplicatedUsing = OnRep_CurrentBushID)
    int32 CurrentBushID = -1;
    
    UPROPERTY(Replicated)
    int32 FactionID = 0;
};