// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BushGameMode.h"
#include "Character/BushCharacter.h"
#include "Player/BushPlayerState.h"
#include "Player/BushPlayerController.h"

ABushGameMode::ABushGameMode()
{
	// Đảm bảo GameMode chỉ tồn tại trên Server
	bReplicates = false;
}

void ABushGameMode::HandlePlayerEnterBush(ABushCharacter* Player, int32 BushID)
{
	if (!HasAuthority() || !Player) return;
    
	// Update server state
	if (ABushPlayerState* PlayerState = Player->GetPlayerState<ABushPlayerState>())
	{
		PlayerState->SetCurrentBushID(BushID);
	}
    
	// Add to bush tracking
	PlayersInBushMap.FindOrAdd(BushID).Players.Add(Player);
    
	// Recalculate visibility for affected bush
	CalculateAndUpdateVisibility(BushID);
    
	UE_LOG(LogTemp, Log, TEXT("Server: Player %s entered Bush %d"), 
		   *Player->GetName(), BushID);
}

void ABushGameMode::HandlePlayerExitBush(ABushCharacter* Player, int32 BushID)
{
	if (!HasAuthority() || !Player) return;
    
	// Update server state
	if (ABushPlayerState* PlayerState = Player->GetPlayerState<ABushPlayerState>())
	{
		PlayerState->SetCurrentBushID(-1); // -1 = not in any bush
	}
    
	// Remove from bush tracking
	if (PlayersInBushMap.Contains(BushID))
	{
		PlayersInBushMap[BushID].Players.Remove(Player);
	}
    
	// Recalculate visibility for affected bush
	CalculateAndUpdateVisibility(BushID);
    
	UE_LOG(LogTemp, Log, TEXT("Server: Player %s exited Bush %d"), 
		   *Player->GetName(), BushID);
}

void ABushGameMode::RegisterBushVolume(ABushVolume* BushVolume, int32 BushID)
{
	if (!HasAuthority() || !BushVolume) return;
    
	BushVolumeMap.Add(BushID, BushVolume);
	UE_LOG(LogTemp, Log, TEXT("Server: Registered Bush Volume %d"), BushID);
}

void ABushGameMode::CalculateAndUpdateVisibility(int32 AffectedBushID)
{
	if (!HasAuthority()) return;
    
	// Iterate through all player controllers
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABushPlayerController* PC = Cast<ABushPlayerController>(*It))
		{
			if (ABushCharacter* ViewerChar = Cast<ABushCharacter>(PC->GetPawn()))
			{
				ABushPlayerState* ViewerState = ViewerChar->GetPlayerState<ABushPlayerState>();
				if (!ViewerState) continue;
                
				// Check visibility for all other players
				for (FConstPlayerControllerIterator It2 = GetWorld()->GetPlayerControllerIterator(); It2; ++It2)
				{
					if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(*It2))
					{
						if (ABushCharacter* OtherChar = Cast<ABushCharacter>(OtherPC->GetPawn()))
						{
							if (OtherChar != ViewerChar)
							{
								ABushPlayerState* OtherState = OtherChar->GetPlayerState<ABushPlayerState>();
								if (!OtherState) continue;
                                
								bool bShouldHide = !CanPlayersSeeEachOther(ViewerState, OtherState);
                                
								// Send to specific client
								PC->Client_UpdateCharacterVisibility(OtherChar, bShouldHide);
							}
						}
					}
				}
			}
		}
	}
}

bool ABushGameMode::CanPlayersSeeEachOther(class ABushPlayerState* Viewer, class ABushPlayerState* Target) const
{
	if (!Viewer || !Target) return true;
    
	int32 ViewerBushID = Viewer->GetCurrentBushID();
	int32 TargetBushID = Target->GetCurrentBushID();
    
	// Both outside bushes = can see
	if (ViewerBushID == -1 && TargetBushID == -1) return true;
    
	// Same bush = can see
	if (ViewerBushID == TargetBushID && ViewerBushID != -1) return true;
    
	// Target in bush, viewer outside = cannot see
	if (ViewerBushID == -1 && TargetBushID != -1) return false;
    
	// Viewer in bush, target outside = can see
	if (ViewerBushID != -1 && TargetBushID == -1) return true;
    
	// Different bushes = cannot see
	return false;
}


