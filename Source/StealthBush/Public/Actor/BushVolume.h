// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BushVolume.generated.h"

class UBoxComponent;
class ABushCharacter;

UCLASS()
class STEALTHBUSH_API ABushVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABushVolume();

	// Bush management
	void AddPlayerToBush(ABushCharacter* Player);
	
	void RemovePlayerFromBush(ABushCharacter* Player);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionBox;

	/** List of players currently inside this bush volume (Server authoritative) */
	UPROPERTY() // Not Replicated - Server authoritative list
	TArray<ABushCharacter*> PlayersInBush;
	
public:
	/** Returns the list of players currently inside this bush (Server only) */
	const TArray<ABushCharacter*>& GetPlayersInBush() const { return PlayersInBush; }
};
