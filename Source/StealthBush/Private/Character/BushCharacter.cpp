// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BushCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Actor/BushVolume.h"

// Sets default values
ABushCharacter::ABushCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;
    
	// Enable replication
	bReplicates = true;
    
	// Cấu hình Capsule Component để phát hiện overlap với BushActor
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	// Initialize
	CurrentBush = nullptr;
}

void ABushCharacter::SetCurrentBush(ABushVolume* NewBush)
{
	if (!HasAuthority()) return;
    
	// Only update if changed
	if (CurrentBush != NewBush)
	{
		ABushVolume* OldBush = CurrentBush;
		CurrentBush = NewBush;
        
		UE_LOG(LogTemp, Log, TEXT("Server: Player %s bush changed from %s to %s"), 
			   *GetName(), 
			   OldBush ? *OldBush->GetName() : TEXT("None"),
			   CurrentBush ? *CurrentBush->GetName() : TEXT("None"));
	}
}

// Called when the game starts or when spawned
void ABushCharacter::BeginPlay()
{
	Super::BeginPlay();
}

