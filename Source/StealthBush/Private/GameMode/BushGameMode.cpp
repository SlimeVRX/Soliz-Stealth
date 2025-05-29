// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BushGameMode.h"
#include "Character/BushCharacter.h"
#include "Player/BushPlayerController.h"
#include "Actor/BushVolume.h"
#include "Kismet/GameplayStatics.h"

ABushGameMode::ABushGameMode()
{
	PlayerControllerClass = ABushPlayerController::StaticClass();
	DefaultPawnClass = ABushCharacter::StaticClass();
}

void ABushGameMode::RegisterBushVolume(ABushVolume* BushVolume)
{
	// This function should only be called on the server
	if (!HasAuthority()) return;

	if (BushVolume && !BushVolumes.Contains(BushVolume))
	{
		BushVolumes.Add(BushVolume);
		UE_LOG(LogTemp, Warning, TEXT("Server: Registered BushVolume: %s. Total registered: %d"), *BushVolume->GetName(), BushVolumes.Num());
	}
}

/*
void ABushGameMode::UpdateVisibilityForPlayerEnterBush(ABushCharacter* AffectedPlayer, ABushVolume* AffectedBush)
{
	if (!HasAuthority() || !AffectedPlayer || !AffectedBush) return;
    
    UE_LOG(LogTemp, Warning, TEXT("PSERVER: Updating visibility for player %s entering bush %s"), 
           *AffectedPlayer->GetName(), *AffectedBush->GetName());
    
    // Lấy tất cả người chơi trong game
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABushCharacter::StaticClass(), AllActors);
    
    ABushPlayerController* PlayerPC = Cast<ABushPlayerController>(AffectedPlayer->GetController());
    if (!PlayerPC) return;
    
    // Phân loại người chơi theo vị trí
    TArray<ABushCharacter*> PlayersOutsideBush;
    TArray<ABushCharacter*> PlayersInSameBush;
    TArray<ABushCharacter*> PlayersInOtherBushes;
    
    for (AActor* OtherActor : AllActors)
    {
        ABushCharacter* OtherPlayer = Cast<ABushCharacter>(OtherActor);
        if (!OtherPlayer || OtherPlayer == AffectedPlayer) continue;
        
        ABushVolume* OtherBush = OtherPlayer->GetCurrentBush();
        
        if (!OtherBush)
        {
            PlayersOutsideBush.Add(OtherPlayer);
        }
        else if (OtherBush == AffectedBush)
        {
            PlayersInSameBush.Add(OtherPlayer);
        }
        else
        {
            PlayersInOtherBushes.Add(OtherPlayer);
        }
    }
    
    // 1. Cập nhật hiển thị cho P
    // P có thể thấy: Tất cả nhân vật ngoài bụi cỏ + Tất cả nhân vật trong bụi cỏ X
    for (ABushCharacter* OutsidePlayer : PlayersOutsideBush)
    {
        PlayerPC->Client_UpdateCharacterVisibility(OutsidePlayer, false); // Hiện
        UE_LOG(LogTemp, Warning, TEXT("PSERVER: Player %s CAN SEE outside player %s"), 
               *AffectedPlayer->GetName(), *OutsidePlayer->GetName());
    }
    
    for (ABushCharacter* SameBushPlayer : PlayersInSameBush)
    {
        PlayerPC->Client_UpdateCharacterVisibility(SameBushPlayer, false); // Hiện
        UE_LOG(LogTemp, Warning, TEXT("PSERVER: Player %s CAN SEE same bush player %s"), 
               *AffectedPlayer->GetName(), *SameBushPlayer->GetName());
    }
    
    // P không thể thấy: Tất cả nhân vật trong các bụi cỏ khác
    for (ABushCharacter* OtherBushPlayer : PlayersInOtherBushes)
    {
        PlayerPC->Client_UpdateCharacterVisibility(OtherBushPlayer, true); // Ẩn
        UE_LOG(LogTemp, Warning, TEXT("PSERVER: Player %s CANNOT SEE other bush player %s"), 
               *AffectedPlayer->GetName(), *OtherBushPlayer->GetName());
    }
    
    // 2. Cập nhật hiển thị của các nhân vật khác đối với P
    // Nhân vật trong bụi cỏ X có thể thấy P
    for (ABushCharacter* SameBushPlayer : PlayersInSameBush)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(SameBushPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, false); // Hiện
            UE_LOG(LogTemp, Warning, TEXT("PSERVER: Same bush player %s CAN SEE Player %s"), 
                   *SameBushPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    // Nhân vật ngoài bụi cỏ không thể thấy P
    for (ABushCharacter* OutsidePlayer : PlayersOutsideBush)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(OutsidePlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, true); // Ẩn
            UE_LOG(LogTemp, Warning, TEXT("PSERVER: Outside player %s CANNOT SEE Player %s"), 
                   *OutsidePlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    // Nhân vật trong các bụi cỏ khác không thể thấy P
    for (ABushCharacter* OtherBushPlayer : PlayersInOtherBushes)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(OtherBushPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, true); // Ẩn
            UE_LOG(LogTemp, Warning, TEXT("PSERVER: Other bush player %s CANNOT SEE Player %s"), 
                   *OtherBushPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("PSERVER: Visibility update complete for player entering bush"));
}

void ABushGameMode::UpdateVisibilityForPlayerExitBush(ABushCharacter* AffectedPlayer)
{
	if (!HasAuthority() || !AffectedPlayer) return;
    
    UE_LOG(LogTemp, Warning, TEXT("PSERVER: Updating visibility for player %s exiting bush"), 
           *AffectedPlayer->GetName());
    
    // Lấy tất cả người chơi trong game
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABushCharacter::StaticClass(), AllActors);
    
    ABushPlayerController* PlayerPC = Cast<ABushPlayerController>(AffectedPlayer->GetController());
    if (!PlayerPC) return;
    
    // Phân loại người chơi theo vị trí
    TArray<ABushCharacter*> PlayersOutsideBush;
    TArray<ABushCharacter*> PlayersInBushes;
    
    for (AActor* OtherActor : AllActors)
    {
        ABushCharacter* OtherPlayer = Cast<ABushCharacter>(OtherActor);
        if (!OtherPlayer || OtherPlayer == AffectedPlayer) continue;
        
        ABushVolume* OtherBush = OtherPlayer->GetCurrentBush();
        
        if (!OtherBush)
        {
            PlayersOutsideBush.Add(OtherPlayer);
        }
        else
        {
            PlayersInBushes.Add(OtherPlayer);
        }
    }
    
    // 1. Cập nhật hiển thị cho P
    // P có thể thấy: Tất cả nhân vật ngoài bụi cỏ
    for (ABushCharacter* OutsidePlayer : PlayersOutsideBush)
    {
        PlayerPC->Client_UpdateCharacterVisibility(OutsidePlayer, false); // Hiện
        UE_LOG(LogTemp, Warning, TEXT("PSERVER: Player %s CAN SEE outside player %s"), 
               *AffectedPlayer->GetName(), *OutsidePlayer->GetName());
    }
    
    // P không thể thấy: Tất cả nhân vật trong bất kỳ bụi cỏ nào
    for (ABushCharacter* BushPlayer : PlayersInBushes)
    {
        PlayerPC->Client_UpdateCharacterVisibility(BushPlayer, true); // Ẩn
        UE_LOG(LogTemp, Warning, TEXT("PSERVER: Player %s CANNOT SEE bush player %s"), 
               *AffectedPlayer->GetName(), *BushPlayer->GetName());
    }
    
    // 2. Cập nhật hiển thị của các nhân vật khác đối với P
    // Tất cả nhân vật đều có thể thấy P khi P ra khỏi bụi cỏ
    for (ABushCharacter* OtherPlayer : PlayersOutsideBush)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(OtherPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, false); // Hiện
            UE_LOG(LogTemp, Warning, TEXT("PSERVER: Outside player %s CAN SEE Player %s"), 
                   *OtherPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    for (ABushCharacter* BushPlayer : PlayersInBushes)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(BushPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, false); // Hiện
            UE_LOG(LogTemp, Warning, TEXT("PSERVER: Bush player %s CAN SEE Player %s"), 
                   *BushPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("PSERVER: Visibility update complete for player exiting bush"));
}
*/

/*void ABushGameMode::HandlePlayerEnteredBush(ABushCharacter* AffectedPlayer, ABushVolume* AffectedBush)
{
	if (!AffectedPlayer || !AffectedBush) 
	{
		UE_LOG(LogTemp, Error, TEXT("SERVER: HandlePlayerEnteredBush received null parameters!"));
		return;
	}
    
	// Thêm Character vào danh sách trong bụi cỏ
	AffectedBush->AddPlayerToBush(AffectedPlayer);

	// Update player's current bush
	AffectedPlayer->SetCurrentBush(AffectedBush);
    
	// Cập nhật visibility
	UpdateVisibilityForPlayerEnterBush(AffectedPlayer, AffectedBush);
}*/

void ABushGameMode::HandlePlayerEnteredBush(ABushCharacter* PlayerCharacter, ABushVolume* Bush)
{
	// This function is called on the server when a player reports entering a bush
	if (!HasAuthority() || !PlayerCharacter || !Bush) return;

	UE_LOG(LogTemp, Warning, TEXT("Server: GameMode handling %s entering bush %s"), *PlayerCharacter->GetName(), *Bush->GetName());

	// 1. Update the server-authoritative state of the character and the bush
	PlayerCharacter->SetCurrentBush(Bush); // Update character's bush pointer on server
	Bush->AddPlayerToBush(PlayerCharacter);       // Add player to bush's list on server

	// 2. Recalculate and update visibility for all relevant players
	UpdateAllPlayerVisibilities();
}

/*void ABushGameMode::HandlePlayerExitedBush(ABushCharacter* AffectedPlayer, ABushVolume* AffectedBush)
{
	if (!AffectedPlayer || !AffectedBush) return;

	// Xóa Character khỏi danh sách trong bụi cỏ
	AffectedBush->RemovePlayerFromBush(AffectedPlayer);

	// Clear player's current bush if it's this bush
	if (AffectedPlayer->GetCurrentBush() == AffectedBush)
	{
		AffectedPlayer->SetCurrentBush(nullptr);
	}

	UpdateVisibilityForPlayerExitBush(AffectedPlayer);
}*/

void ABushGameMode::HandlePlayerExitedBush(ABushCharacter* PlayerCharacter, ABushVolume* Bush)
{
	// This function is called on the server when a player reports exiting a bush
	if (!HasAuthority() || !PlayerCharacter || !Bush) return;

	UE_LOG(LogTemp, Warning, TEXT("Server: GameMode handling %s exiting bush %s"), *PlayerCharacter->GetName(), *Bush->GetName());

	// 1. Update the server-authoritative state of the character and the bush
	PlayerCharacter->SetCurrentBush(nullptr); // Update character's bush pointer on server
	Bush->RemovePlayerFromBush(PlayerCharacter);     // Remove player from bush's list on server

	// 2. Recalculate and update visibility for all relevant players
	UpdateAllPlayerVisibilities();
}

/*
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
*/

/*
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
*/

void ABushGameMode::UpdateAllPlayerVisibilities()
{
	// This function recalculates visibility for ALL players and sends necessary updates.
	// This might be inefficient for large numbers of players/bushes; optimization would involve
	// only updating players affected by the change (e.g., players near the entered/exited bush,
	// and the entering/exiting player themselves).
	if (!HasAuthority()) return; // Only run on server

	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABushCharacter::StaticClass(), PlayerActors);

	TArray<ABushCharacter*> AllCharacters;
	for (AActor* PlayerActor : PlayerActors)
	{
		if (ABushCharacter* Char = Cast<ABushCharacter>(PlayerActor))
		{
			AllCharacters.Add(Char);
		}
	}

	// Iterate through every pair of players (Viewer and Target)
	for (ABushCharacter* Viewer : AllCharacters)
	{
		if (!Viewer || !Viewer->GetController()) continue;

		ABushPlayerController* ViewerPC = Cast<ABushPlayerController>(Viewer->GetController());
		if (!ViewerPC) continue;

		for (ABushCharacter* Target : AllCharacters)
		{
			if (!Target) continue;

			// A player should always see themselves
			bool bShouldBeHiddenToViewer = (Viewer != Target) && !CanPlayersSeeEachOther(Viewer, Target);

			// Send the visibility update to the Viewer's client
			SendVisibilityUpdate(ViewerPC, Target, bShouldBeHiddenToViewer);
		}
	}
}

bool ABushGameMode::CanPlayersSeeEachOther(ABushCharacter* ViewerCharacter, ABushCharacter* TargetCharacter) const
{
	// --- CORE STEALTH LOGIC GOES HERE ---
	// This is a placeholder implementation. Implement your actual game rules here.
	// Examples of rules:
	// - If TargetCharacter is in ANY bush, ViewerCharacter cannot see them unless ViewerCharacter is also in the SAME bush.
	// - If ViewerCharacter is in a bush, they cannot see anyone outside.
	// - If neither is in a bush, they see each other.
	// - Line of sight checks, distance checks, specific bush types, etc.

	if (!ViewerCharacter || !TargetCharacter || ViewerCharacter == TargetCharacter)
	{
		// A character always sees themselves. Invalid inputs.
		return true;
	}

	bool bViewerIsInBush = ViewerCharacter->IsInBush(); // IsInBush is accurate on server
	ABushVolume* ViewerBush = ViewerCharacter->GetCurrentBush(); // GetCurrentBush is accurate on server

	bool bTargetIsInBush = TargetCharacter->IsInBush(); // IsInBush is accurate on server
	ABushVolume* TargetBush = TargetCharacter->GetCurrentBush(); // GetCurrentBush is accurate on server


	// Example Rule:
	// - If Target is in ANY bush, AND Viewer is NOT in the SAME bush, Target is hidden from Viewer.
	// - Otherwise, Target is visible to Viewer.
	if (bTargetIsInBush)
	{
		// Target is in a bush
		if (bViewerIsInBush && ViewerBush == TargetBush)
		{
			// Viewer is in the SAME bush -> Viewer CAN see Target
			return true;
		}
		else
		{
			// Viewer is NOT in the SAME bush (either in a different bush or outside)
			// -> Viewer CANNOT see Target
			return false; // Target should be hidden
		}
	}
	else
	{
		// Target is NOT in a bush -> Viewer CAN always see Target (assuming no other rules like distance/LOS)
		return true; // Target is visible
	}

	// Add more complex rules as needed, e.g., line of sight, distance, etc.
}

void ABushGameMode::SendVisibilityUpdate(APlayerController* ReceivingPC, ABushCharacter* TargetCharacter, bool bShouldBeHidden)
{
	if (!ReceivingPC || !TargetCharacter) return;

	// Cast the PlayerController to our custom BushPlayerController
	if (ABushPlayerController* BushPC = Cast<ABushPlayerController>(ReceivingPC))
	{
		// Call the Client RPC on this PlayerController's instance on the target client
		BushPC->Client_UpdateCharacterVisibility(TargetCharacter, bShouldBeHidden);
	}
}