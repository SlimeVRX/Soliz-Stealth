// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BushCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Actor/BushVolume.h"
#include "Player/BushPlayerController.h"

// Sets default values
ABushCharacter::ABushCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;
	
	// Set network properties
	bReplicates = true;
    
	// Cấu hình Capsule Component để phát hiện overlap với BushActor
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	// Initialize
	CurrentBush = nullptr;
}

// Called when the game starts or when spawned
void ABushCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Register overlap events
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABushCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABushCharacter::OnOverlapEnd);
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

void ABushCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Kiểm tra xem Actor va chạm có phải là ABushVolume không
	ABushVolume* Bush = Cast<ABushVolume>(OtherActor);
	
	if (Bush && IsLocallyControlled())
	{
		UE_LOG(LogTemp, Log, TEXT("Character %s overlapped with Bush %s"), 
			   *GetName(), *Bush->GetName());

		// Thông báo cho PlayerController
		ABushPlayerController* PC = Cast<ABushPlayerController>(Controller);
		if (PC)
		{
			UE_LOG(LogTemp, Log, TEXT("CLIENT %s: Reporting Enter to Server for Bush %s"), 
				   *GetName(), *Bush->GetName());
			PC->Server_ReportEnteredBush(Bush);
		}
	}
}

void ABushCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Kiểm tra xem Actor va chạm có phải là ABushVolume không
	ABushVolume* Bush = Cast<ABushVolume>(OtherActor);
	
	if (Bush && IsLocallyControlled())
	{
		// Thông báo cho PlayerController
		ABushPlayerController* PC = Cast<ABushPlayerController>(Controller);
		if (PC)
		{
			UE_LOG(LogTemp, Log, TEXT("CLIENT %s: Reporting Exit to Server for Bush %s"), 
				   *GetName(), *Bush->GetName());
			PC->Server_ReportExitedBush(Bush);
		}
	}
}

