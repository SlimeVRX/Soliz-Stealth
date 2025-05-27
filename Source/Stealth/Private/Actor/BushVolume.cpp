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
			GameMode->RegisterBushVolume(this, BushID);
		}
	}
}

void ABushVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only detect on server
	if (!HasAuthority()) return;
    
	if (ABushCharacter* BushChar = Cast<ABushCharacter>(OtherActor))
	{
		if (ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>())
		{
			GameMode->HandlePlayerEnterBush(BushChar, BushID);
		}
	}
}

void ABushVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Only detect on server
	if (!HasAuthority()) return;
    
	if (ABushCharacter* BushChar = Cast<ABushCharacter>(OtherActor))
	{
		if (ABushGameMode* GameMode = GetWorld()->GetAuthGameMode<ABushGameMode>())
		{
			GameMode->HandlePlayerExitBush(BushChar, BushID);
		}
	}
}

