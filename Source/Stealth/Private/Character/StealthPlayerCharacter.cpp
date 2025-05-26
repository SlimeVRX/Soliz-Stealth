// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/StealthPlayerCharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AStealthPlayerCharacter::AStealthPlayerCharacter()
{
	// Cài đặt Character này được replicate
	SetReplicates(true);
	SetReplicateMovement(true);
    
	// Đảm bảo Character luôn được coi là relevant cho network
	bAlwaysRelevant = true;
    
	// Cấu hình Capsule Component để phát hiện overlap với BushActor
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

}

// Called when the game starts or when spawned
void AStealthPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Log thông tin khi spawn
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("CustomPlayerCharacter spawned on Server: %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CustomPlayerCharacter spawned on Client: %s"), *GetName());
	}
}