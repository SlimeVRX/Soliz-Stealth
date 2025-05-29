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

void ABushGameMode::UpdateVisibilityForPlayerEnterBush(ABushCharacter* AffectedPlayer, ABushVolume* AffectedBush)
{
	if (!HasAuthority() || !AffectedPlayer || !AffectedBush) return;
    
    UE_LOG(LogTemp, Warning, TEXT("SERVER: Updating visibility for player %s entering bush %s"), 
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
        UE_LOG(LogTemp, Verbose, TEXT("SERVER: Player %s CAN SEE outside player %s"), 
               *AffectedPlayer->GetName(), *OutsidePlayer->GetName());
    }
    
    for (ABushCharacter* SameBushPlayer : PlayersInSameBush)
    {
        PlayerPC->Client_UpdateCharacterVisibility(SameBushPlayer, false); // Hiện
        UE_LOG(LogTemp, Verbose, TEXT("SERVER: Player %s CAN SEE same bush player %s"), 
               *AffectedPlayer->GetName(), *SameBushPlayer->GetName());
    }
    
    // P không thể thấy: Tất cả nhân vật trong các bụi cỏ khác
    for (ABushCharacter* OtherBushPlayer : PlayersInOtherBushes)
    {
        PlayerPC->Client_UpdateCharacterVisibility(OtherBushPlayer, true); // Ẩn
        UE_LOG(LogTemp, Verbose, TEXT("SERVER: Player %s CANNOT SEE other bush player %s"), 
               *AffectedPlayer->GetName(), *OtherBushPlayer->GetName());
    }
    
    // 2. Cập nhật hiển thị của các nhân vật khác đối với P
    // Nhân vật trong bụi cỏ X có thể thấy P
    for (ABushCharacter* SameBushPlayer : PlayersInSameBush)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(SameBushPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, false); // Hiện
            UE_LOG(LogTemp, Verbose, TEXT("SERVER: Same bush player %s CAN SEE Player %s"), 
                   *SameBushPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    // Nhân vật ngoài bụi cỏ không thể thấy P
    for (ABushCharacter* OutsidePlayer : PlayersOutsideBush)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(OutsidePlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, true); // Ẩn
            UE_LOG(LogTemp, Verbose, TEXT("SERVER: Outside player %s CANNOT SEE Player %s"), 
                   *OutsidePlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    // Nhân vật trong các bụi cỏ khác không thể thấy P
    for (ABushCharacter* OtherBushPlayer : PlayersInOtherBushes)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(OtherBushPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, true); // Ẩn
            UE_LOG(LogTemp, Verbose, TEXT("SERVER: Other bush player %s CANNOT SEE Player %s"), 
                   *OtherBushPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("SERVER: Visibility update complete for player entering bush"));
}

void ABushGameMode::UpdateVisibilityForPlayerExitBush(ABushCharacter* AffectedPlayer)
{
	if (!HasAuthority() || !AffectedPlayer) return;
    
    UE_LOG(LogTemp, Warning, TEXT("SERVER: Updating visibility for player %s exiting bush"), 
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
        UE_LOG(LogTemp, Verbose, TEXT("SERVER: Player %s CAN SEE outside player %s"), 
               *AffectedPlayer->GetName(), *OutsidePlayer->GetName());
    }
    
    // P không thể thấy: Tất cả nhân vật trong bất kỳ bụi cỏ nào
    for (ABushCharacter* BushPlayer : PlayersInBushes)
    {
        PlayerPC->Client_UpdateCharacterVisibility(BushPlayer, true); // Ẩn
        UE_LOG(LogTemp, Verbose, TEXT("SERVER: Player %s CANNOT SEE bush player %s"), 
               *AffectedPlayer->GetName(), *BushPlayer->GetName());
    }
    
    // 2. Cập nhật hiển thị của các nhân vật khác đối với P
    // Tất cả nhân vật đều có thể thấy P khi P ra khỏi bụi cỏ
    for (ABushCharacter* OtherPlayer : PlayersOutsideBush)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(OtherPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, false); // Hiện
            UE_LOG(LogTemp, Verbose, TEXT("SERVER: Outside player %s CAN SEE Player %s"), 
                   *OtherPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    for (ABushCharacter* BushPlayer : PlayersInBushes)
    {
        if (ABushPlayerController* OtherPC = Cast<ABushPlayerController>(BushPlayer->GetController()))
        {
            OtherPC->Client_UpdateCharacterVisibility(AffectedPlayer, false); // Hiện
            UE_LOG(LogTemp, Verbose, TEXT("SERVER: Bush player %s CAN SEE Player %s"), 
                   *BushPlayer->GetName(), *AffectedPlayer->GetName());
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("SERVER: Visibility update complete for player exiting bush"));
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



