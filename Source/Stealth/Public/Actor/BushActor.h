// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "BushActor.generated.h"

UCLASS()
class STEALTH_API ABushActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABushActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Box component để phát hiện va chạm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bush")
	UBoxComponent* BushOverlapVolume;

	// ID của bụi cỏ (để debug)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bush")
	int32 BushID;
    
	// Xử lý sự kiện va chạm
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
    
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
					 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
                     
	// Multicast RPC để thông báo cho client
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NotifyBushOverlap(AStealthPlayerCharacter* Player, bool bEntered);
};
