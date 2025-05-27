// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BushCharacter.h"
#include "Player/BushPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABushCharacter::ABushCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;
    
	// Enable replication
	bReplicates = true;
    
	// Cấu hình Capsule Component để phát hiện overlap với BushActor
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

}

bool ABushCharacter::IsInBush() const
{
	if (ABushPlayerState* BushPS = GetPlayerState<ABushPlayerState>())
	{
		return BushPS->GetCurrentBushID() != -1;
	}
	return false;
}

int32 ABushCharacter::GetCurrentBushID() const
{
	if (ABushPlayerState* BushPS = GetPlayerState<ABushPlayerState>())
	{
		return BushPS->GetCurrentBushID();
	}
	return -1;
}

// Called when the game starts or when spawned
void ABushCharacter::BeginPlay()
{
	Super::BeginPlay();
}

