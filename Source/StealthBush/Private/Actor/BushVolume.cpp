// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BushVolume.h"
#include "Components/BoxComponent.h"
#include "Character/BushCharacter.h"
#include "GameMode/BushGameMode.h"


// Sets default values
ABushVolume::ABushVolume()
{
	PrimaryActorTick.bCanEverTick = false;
    
	// Create collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	CollisionBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
    
	// Set network properties
	bReplicates = true;
}

void ABushVolume::AddPlayerToBush(ABushCharacter* Player)
{
	if (!HasAuthority() || !Player) return;
    
	PlayersInBush.Add(Player);
	UE_LOG(LogTemp, Log, TEXT("Bush %s: Added player %s, total players: %d"), 
		   *GetName(), *Player->GetName(), PlayersInBush.Num());
}

void ABushVolume::RemovePlayerFromBush(ABushCharacter* Player)
{
	if (!HasAuthority() || !Player) return;
    
	PlayersInBush.Remove(Player);
	UE_LOG(LogTemp, Log, TEXT("Bush %s: Removed player %s, total players: %d"), 
		   *GetName(), *Player->GetName(), PlayersInBush.Num());
}

// Called when the game starts or when spawned
void ABushVolume::BeginPlay()
{
	Super::BeginPlay();
    
	// Register with GameMode (Server only)
	if (HasAuthority())
	{
		if (ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>())
		{
			GameMode->RegisterBushVolume(this);
		}
	}
}

