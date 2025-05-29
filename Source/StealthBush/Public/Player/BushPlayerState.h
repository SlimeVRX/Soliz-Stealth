// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BushPlayerState.generated.h"

/**
 * Player state with bush system data
 */
UCLASS()
class STEALTHBUSH_API ABushPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ABushPlayerState();
};