// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BushCharacter.generated.h"

/**
 * Character with bush system support
 */
UCLASS()
class STEALTHBUSH_API ABushCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABushCharacter();

	// Getters
	UFUNCTION(BlueprintPure, Category = "Bush System")
	bool IsInBush() const;
    
	UFUNCTION(BlueprintPure, Category = "Bush System")
	int32 GetCurrentBushID() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
