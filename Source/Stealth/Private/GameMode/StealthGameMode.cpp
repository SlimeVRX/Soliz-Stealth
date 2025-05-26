// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/StealthGameMode.h"
#include "Manager/BushVisibilityManager.h"
#include "Character/StealthPlayerCharacter.h"

AStealthGameMode::AStealthGameMode()
{
	// Đảm bảo GameMode chỉ tồn tại trên Server
	bReplicates = false;
}

void AStealthGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Chỉ tạo BushVisibilityManager trên Server
	if (HasAuthority())
	{
		// Tạo BushVisibilityManager
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		BushVisibilityManager = GetWorld()->SpawnActor<ABushVisibilityManager>(
			ABushVisibilityManager::StaticClass(), 
			FVector::ZeroVector, 
			FRotator::ZeroRotator, 
			SpawnParams);
        
		if (BushVisibilityManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("BushVisibilityManager created successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create BushVisibilityManager"));
		}
	}
}
