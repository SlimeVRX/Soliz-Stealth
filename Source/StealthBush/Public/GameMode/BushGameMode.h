// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BushGameMode.generated.h"

class ABushCharacter;
class ABushVolume;
class ABushPlayerController;

/**
 * 
 */
UCLASS()
class STEALTHBUSH_API ABushGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABushGameMode();
    
	// Bush system management
	void RegisterBushVolume(ABushVolume* BushVolume);

	/*
	// Visibility calculation
	auto UpdateVisibilityForPlayerEnterBush(ABushCharacter* AffectedPlayer, ABushVolume* AffectedBush) -> void;
	void UpdateVisibilityForPlayerExitBush(ABushCharacter* AffectedPlayer);
	*/

	void HandlePlayerEnteredBush(ABushCharacter* AffectedPlayer, ABushVolume* AffectedBush);
	void HandlePlayerExitedBush(ABushCharacter* AffectedPlayer, ABushVolume* AffectedBush);
	
protected:
	// Collection of all bush volumes
	UPROPERTY()
	TArray<ABushVolume*> BushVolumes;
	
	/*// Visibility rules
	bool CanPlayersSeeEachOther(ABushCharacter* Viewer, ABushCharacter* Target) const;
	
	// Helper to update visibility between two players
	void UpdateVisibilityBetweenPlayers(ABushCharacter* Viewer, ABushCharacter* Target);*/

	
	bool CanPlayersSeeEachOther(ABushCharacter* ViewerCharacter, ABushCharacter* TargetCharacter) const;
	
	void UpdateAllPlayerVisibilities();
	
	void SendVisibilityUpdate(APlayerController* ReceivingPC, ABushCharacter* TargetCharacter, bool bShouldBeHidden);
};
