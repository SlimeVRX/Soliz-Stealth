// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BushCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Actor/BushVolume.h"
#include "Player/BushPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABushCharacter::ABushCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
	
	// Set network properties
	bReplicates = true;

	GetCharacterMovement()->SetIsReplicated(true);
    
	// Configure CapsuleComponent for overlap events
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Or QueryOnly if no physics interaction needed
	// Ensure CapsuleComponent overlaps with BushVolume's collision channel (e.g., WorldDynamic or custom channel)
	// By default, Pawn capsule overlaps with WorldDynamic.
	// We configured BushVolume's Box to overlap with Pawn. Let's explicitly ensure Pawn overlaps with WorldDynamic as well.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Ensure overlap with other pawns
	GetCapsuleComponent()->SetGenerateOverlapEvents(true); // Crucial!
}

// Called when the game starts or when spawned
void ABushCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Register overlap events
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABushCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABushCharacter::OnOverlapEnd);

	// Initialize
	CurrentBush = nullptr;
}

void ABushCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is a BushVolume
	ABushVolume* BushVolume = Cast<ABushVolume>(OtherActor);
	if (BushVolume)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character overlapped with BushVolume: %s"), *BushVolume->GetName());

		// IMPORTANT: Overlap events fire on both server and client.
		// We only want the client controlling this character to report the entry to the server.
		// The server's instance of this character doesn't need to process overlap this way;
		// the server will receive the update via RPC from the client.
		if (IsLocallyControlled())
		{
			UE_LOG(LogTemp, Warning, TEXT("Local client controlled character entering bush. Reporting to server..."));
			if (ABushPlayerController* PC = Cast<ABushPlayerController>(GetController()))
			{
				PC->Server_ReportEnteredBush(BushVolume);
			}
		}
	}
}

void ABushCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if the exiting actor is a BushVolume
	ABushVolume* BushVolume = Cast<ABushVolume>(OtherActor);
	if (BushVolume)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character stopped overlapping with BushVolume: %s"), *BushVolume->GetName());

		// IMPORTANT: Similar to begin overlap, only report from the locally controlled client.
		if (IsLocallyControlled())
		{
			UE_LOG(LogTemp, Warning, TEXT("Local client controlled character exiting bush. Reporting to server..."));
			if (ABushPlayerController* PC = Cast<ABushPlayerController>(GetController()))
			{
				PC->Server_ReportExitedBush(BushVolume);
			}
		}
	}
}

void ABushCharacter::SetCurrentBush(ABushVolume* NewBush)
{
	if (!HasAuthority()) return;

	if (CurrentBush != NewBush)
	{
		ABushVolume* OldBush = CurrentBush;
		CurrentBush = NewBush;

		UE_LOG(LogTemp, Log, TEXT("Server: %s changed bush from %s to %s"),
			*GetName(),
			OldBush ? *OldBush->GetName() : TEXT("None"),
			NewBush ? *NewBush->GetName() : TEXT("None"));
	}
}

void ABushCharacter::SetCharacterVisibility(bool bShouldBeHidden)
{
	// This function is called by ABushPlayerController on the client (via Client RPC)
	// to update the visibility of this specific character actor on the local client's machine.
	// It affects only the rendering, not the server-side state.
	SetActorHiddenInGame(bShouldBeHidden);
	UE_LOG(LogTemp, Warning, TEXT("Client: Character %s visibility set to hidden=%s"), *GetName(), bShouldBeHidden ? TEXT("true") : TEXT("false"));
}