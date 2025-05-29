// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BushGameMode.generated.h"

class ABushCharacter;
class ABushVolume;

USTRUCT()
struct FBushPlayerSet
{
	GENERATED_BODY()
    
	UPROPERTY()
	TSet<ABushCharacter*> Players;
};

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
	UFUNCTION()
	void HandlePlayerEnterBush(ABushCharacter* Player, int32 BushID);
    
	UFUNCTION()
	void HandlePlayerExitBush(ABushCharacter* Player, int32 BushID);
    
	UFUNCTION()
	void RegisterBushVolume(ABushVolume* BushVolume, int32 BushID);
	
private:
	// Map quản lý tất cả bushes
	UPROPERTY()
	TMap<int32, ABushVolume*> BushVolumeMap;
    
	// Map tracking players trong từng bush
	UPROPERTY()
	TMap<int32, FBushPlayerSet> PlayersInBushMap;
    
	// Server visibility calculation
	void CalculateAndUpdateVisibility(int32 AffectedBushID);
    
	// Visibility rules
	bool CanPlayersSeeEachOther(class ABushPlayerState* Viewer, class ABushPlayerState* Target) const;
};
