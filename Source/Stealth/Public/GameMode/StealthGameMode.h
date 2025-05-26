// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StealthGameMode.generated.h"

class ABushVisibilityManager;

/**
 * 
 */
UCLASS()
class STEALTH_API AStealthGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStealthGameMode();

	// Getter cho BushVisibilityManager
	UFUNCTION(BlueprintCallable, Category = "Bush System")
	ABushVisibilityManager* GetBushVisibilityManager() const { return BushVisibilityManager; }

protected:
	virtual void BeginPlay() override;
    
	// Tham chiếu đến BushVisibilityManager
	UPROPERTY()
	ABushVisibilityManager* BushVisibilityManager;
};
