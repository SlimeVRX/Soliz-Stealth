// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BushCharacter.generated.h"

class ABushVolume;

/**
 * Character with bush system support
 */
UCLASS()
class STEALTHBUSH_API ABushCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABushCharacter();
	
	// Getters
	UFUNCTION(BlueprintPure, Category = "Bush System")
	bool IsInBush() const { return CurrentBush != nullptr; }
	
	UFUNCTION(BlueprintPure, Category = "Bush System")
	ABushVolume* GetCurrentBush() const { return CurrentBush; }

	// Setters (Server only)
	void SetCurrentBush(ABushVolume* NewBush);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Current bush this character is in (Replicated)
	UPROPERTY()
	ABushVolume* CurrentBush;
	
	// Overlap events
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
					  bool bFromSweep, const FHitResult& SweepResult);
    
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
};
