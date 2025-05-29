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

	// Register overlap events
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABushVolume::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABushVolume::OnOverlapEnd);
    
	// Register with GameMode (Server only)
	if (HasAuthority())
	{
		if (ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>())
		{
			GameMode->RegisterBushVolume(this);
		}
	}
}

void ABushVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only detect on server
	if (!HasAuthority()) return;
    
	if (ABushCharacter* BushChar = Cast<ABushCharacter>(OtherActor))
	{
		// Add player to bush
		AddPlayerToBush(BushChar);
        
		// Update player's current bush
		BushChar->SetCurrentBush(this);

		// Recalculate visibility
		// Notify GameMode to recalculate visibility for this bush
		if (ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>())
		{
			GameMode->CalculateAndUpdateVisibilityForBush(this);
		}
        
		UE_LOG(LogTemp, Log, TEXT("Server: Player %s entered Bush %s"), 
			   *BushChar->GetName(), *GetName());
	}
}

void ABushVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// Only detect on server
	if (!HasAuthority()) return;
    
	if (ABushCharacter* BushChar = Cast<ABushCharacter>(OtherActor))
	{
		// Remove player from bush
		RemovePlayerFromBush(BushChar);
        
		// Clear player's current bush if it's this bush
		if (BushChar->GetCurrentBush() == this)
		{
			BushChar->SetCurrentBush(nullptr);
		}
        
		// Recalculate visibility
		// Notify GameMode to recalculate visibility for this bush
		if (ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>())
		{
			// First update visibility for the bush (for players still in the bush)
			GameMode->CalculateAndUpdateVisibilityForBush(this);

			// Then update visibility specifically for the player who exited
			GameMode->CalculateAndUpdateVisibilityForPlayer(BushChar);
		}
        
		UE_LOG(LogTemp, Log, TEXT("Server: Player %s exited Bush %s"), 
			   *BushChar->GetName(), *GetName());
	}
}

