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

	// Getters
	UFUNCTION(BlueprintPure, Category = "Bush System")
	const TSet<ABushCharacter*>& GetPlayersInBush() const { return PlayersInBush; }

	// Bush management
	void AddPlayerToBush(ABushCharacter* Player);
	void RemovePlayerFromBush(ABushCharacter* Player);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionBox;

	// Collection of players currently in this bush
	UPROPERTY()
	TSet<ABushCharacter*> PlayersInBush;

	// Overlap events
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
    
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
