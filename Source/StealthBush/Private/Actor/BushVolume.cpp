// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BushVolume.h"
#include "Components/BoxComponent.h"
#include "Character/BushCharacter.h"
#include "GameMode/BushGameMode.h"


// Sets default values
ABushVolume::ABushVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// Enable replication for this actor
	bReplicates = true;
	SetReplicateMovement(true); // Replicate position, rotation, scale
    
	// Create collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	// Configure collision settings for the box
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // We only care about querying (overlapping)
	// Set Collision Profile Name - "Trigger" is a common profile for volumes that detect overlaps
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	// Ensure Generate Overlap Events is true
	CollisionBox->SetGenerateOverlapEvents(true);

	// Set specific collision responses if needed (e.g., ignore everything but overlap with Pawn)
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore); // Ignore everything by default
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap with Pawns (Characters)
	CollisionBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
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

void ABushVolume::AddPlayerToBush(ABushCharacter* Player)
{
	if (!HasAuthority()) return;
    
	if (Player && !PlayersInBush.Contains(Player))
	{
		PlayersInBush.Add(Player);
		UE_LOG(LogTemp, Warning, TEXT("Server: Player %s entered Bush %s. Total in bush: %d"), *Player->GetName(), *GetName(), PlayersInBush.Num());
		// Note: GameMode will handle the visibility logic based on this change
	}
}

void ABushVolume::RemovePlayerFromBush(ABushCharacter* Player)
{
	if (!HasAuthority()) return;

	if (Player)
	{
		PlayersInBush.Remove(Player);
		UE_LOG(LogTemp, Warning, TEXT("Server: Player %s exited Bush %s. Total in bush: %d"), *Player->GetName(), *GetName(), PlayersInBush.Num());
		// Note: GameMode will handle the visibility logic based on this change
	}
}

