// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BushVisibilityManager.generated.h"

class AStealthPlayerCharacter;
class ABushActor;

UCLASS()
class STEALTH_API ABushVisibilityManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABushVisibilityManager();

	// Xử lý sự kiện Player vào/ra bụi cỏ
	void HandlePlayerEnteredBush(AStealthPlayerCharacter* Player, ABushActor* Bush);
	void HandlePlayerExitedBush(AStealthPlayerCharacter* Player, ABushActor* Bush);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Map lưu trữ thông tin Player đang ở trong bụi cỏ nào
	UPROPERTY()
	TMap<AStealthPlayerCharacter*, ABushActor*> PlayerToBushMap;

	// Tính toán lại hiển thị cho tất cả Player
	void RecomputeVisibilityForAll();

	// Áp dụng quy tắc hiển thị
	void ApplyVisibilityRules(AStealthPlayerCharacter* Observer, AStealthPlayerCharacter* Target);

	// Kiểm tra xem Target có nên hiển thị với Observer không
	bool ShouldTargetBeVisibleToObserver(AStealthPlayerCharacter* Observer, AStealthPlayerCharacter* Target);

};
