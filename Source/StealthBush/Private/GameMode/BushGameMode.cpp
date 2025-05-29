// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BushGameMode.h"
#include "Character/BushCharacter.h"
#include "Player/BushPlayerController.h"
#include "Actor/BushVolume.h"
#include "Kismet/GameplayStatics.h"

ABushGameMode::ABushGameMode()
{
	// Đảm bảo GameMode chỉ tồn tại trên Server
	bReplicates = false;
}

void ABushGameMode::RegisterBushVolume(ABushVolume* BushVolume)
{
	if (!HasAuthority() || !BushVolume) return;
    
	BushVolumes.AddUnique(BushVolume);
	UE_LOG(LogTemp, Log, TEXT("Server: Registered Bush Volume %s"), *BushVolume->GetName());
}

void ABushGameMode::CalculateAndUpdateVisibilityForBush(ABushVolume* AffectedBush)
{
	if (!HasAuthority() || !AffectedBush) return;
    
	UE_LOG(LogTemp, Warning, TEXT("SERVER: Recalculating visibility for affected Bush: %s"), *AffectedBush->GetName());
    
	// Get players in this bush
	const TSet<ABushCharacter*>& PlayersInBush = AffectedBush->GetPlayersInBush();
    
	// Get all players in the game
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABushCharacter::StaticClass(), AllPlayers);
    
	// For each player in this bush, update visibility with all other players
	for (ABushCharacter* PlayerInBush : PlayersInBush)
	{
		// Update visibility for this player
		CalculateAndUpdateVisibilityForPlayer(PlayerInBush);
	}
    
	// For each player not in this bush, update visibility with players in this bush
	for (AActor* OtherActor : AllPlayers)
	{
		ABushCharacter* OtherPlayer = Cast<ABushCharacter>(OtherActor);
		if (!OtherPlayer || PlayersInBush.Contains(OtherPlayer)) continue;
        
		// Update visibility for this player with all players in the bush
		for (ABushCharacter* PlayerInBush : PlayersInBush)
		{
			UpdateVisibilityBetweenPlayers(OtherPlayer, PlayerInBush);
			UpdateVisibilityBetweenPlayers(PlayerInBush, OtherPlayer);
		}
	}
    
	UE_LOG(LogTemp, Warning, TEXT("SERVER: Visibility recalculation complete"));
}

void ABushGameMode::CalculateAndUpdateVisibilityForPlayer(ABushCharacter* AffectedPlayer)
{
	if (!HasAuthority() || !AffectedPlayer) return;
    
	// Get all players in the game
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABushCharacter::StaticClass(), AllPlayers);
    
	// Update visibility between this player and all other players
	for (AActor* OtherActor : AllPlayers)
	{
		ABushCharacter* OtherPlayer = Cast<ABushCharacter>(OtherActor);
		if (!OtherPlayer || OtherPlayer == AffectedPlayer) continue;

		// Update visibility in BOTH directions
		// 1. Can AffectedPlayer see OtherPlayer?
		UpdateVisibilityBetweenPlayers(AffectedPlayer, OtherPlayer);

		// 2. Can OtherPlayer see AffectedPlayer?
		UpdateVisibilityBetweenPlayers(OtherPlayer, AffectedPlayer);
	}
}

bool ABushGameMode::CanPlayersSeeEachOther(ABushCharacter* Viewer, ABushCharacter* Target) const
{
	if (!Viewer || !Target) 
	{
		UE_LOG(LogTemp, Error, TEXT("SERVER: Invalid Character in visibility check"));
		return true;
	}
    
	ABushVolume* ViewerBush = Viewer->GetCurrentBush();
	ABushVolume* TargetBush = Target->GetCurrentBush();
    
	// Both outside bushes = can see
	if (!ViewerBush && !TargetBush) 
	{
		UE_LOG(LogTemp, Verbose, TEXT("SERVER: Visibility Rule - Both outside bushes = CAN SEE"));
		return true;
	}
    
	// Same bush = can see
	if (ViewerBush && TargetBush && ViewerBush == TargetBush) 
	{
		UE_LOG(LogTemp, Verbose, TEXT("SERVER: Visibility Rule - Same bush = CAN SEE"));
		return true;
	}
    
	// Target in bush, viewer outside = cannot see
	if (!ViewerBush && TargetBush) 
	{
		UE_LOG(LogTemp, Verbose, TEXT("SERVER: Visibility Rule - Target in bush, viewer outside = CANNOT SEE"));
		return false;
	}
    
	// Viewer in bush, target outside = can see
	if (ViewerBush && !TargetBush) 
	{
		UE_LOG(LogTemp, Verbose, TEXT("SERVER: Visibility Rule - Viewer in bush, target outside = CAN SEE"));
		return true;
	}
    
	// Different bushes = cannot see
	UE_LOG(LogTemp, Verbose, TEXT("SERVER: Visibility Rule - Different bushes = CANNOT SEE"));
	return false;
}

void ABushGameMode::UpdateVisibilityBetweenPlayers(ABushCharacter* Viewer, ABushCharacter* Target)
{
	if (!HasAuthority() || !Viewer || !Target || Viewer == Target) return;
    
	// Apply visibility rules
	bool bCanSee = CanPlayersSeeEachOther(Viewer, Target);
    
	// Update visibility on viewer's client
	if (ABushPlayerController* PC = Cast<ABushPlayerController>(Viewer->GetController()))
	{
		PC->Client_UpdateCharacterVisibility(Target, !bCanSee);
        
		UE_LOG(LogTemp, Log, TEXT("SERVER: Visibility check - Viewer: %s -> Target: %s = %s"), 
			   *Viewer->GetName(),
			   *Target->GetName(),
			   bCanSee ? TEXT("VISIBLE") : TEXT("HIDDEN"));
	}
}



