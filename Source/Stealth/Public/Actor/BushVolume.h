// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BushVolume.generated.h"

class UBoxComponent;

UCLASS()
class STEALTH_API ABushVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABushVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Overlap events
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
					   AActor* OtherActor, 
					   UPrimitiveComponent* OtherComp, 
					   int32 OtherBodyIndex, 
					   bool bFromSweep, 
					   const FHitResult& SweepResult);
    
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, 
					 AActor* OtherActor, 
					 UPrimitiveComponent* OtherComp, 
					 int32 OtherBodyIndex);
    
	// Getters
	int32 GetBushID() const { return BushID; }

protected:
	// Collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionBox;
    
	// Bush identifier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bush")
	int32 BushID = 0;
};
