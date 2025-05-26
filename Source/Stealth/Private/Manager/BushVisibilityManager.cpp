// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/BushVisibilityManager.h"
#include "Character/StealthPlayerCharacter.h"
#include "Actor/BushActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABushVisibilityManager::ABushVisibilityManager()
{
	// Manager chỉ tồn tại trên Server, không được replicate
	bReplicates = false;\
	bNetTemporary = true;

	// Tắt tick vì không cần cập nhật mỗi frame
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABushVisibilityManager::BeginPlay()
{
	Super::BeginPlay();

	// Chỉ chạy trên Server
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("BushVisibilityManager initialized on Server"));
	}
}

void ABushVisibilityManager::HandlePlayerEnteredBush(AStealthPlayerCharacter* Player, ABushActor* Bush)
{
	// Chỉ chạy trên Server
	if (!HasAuthority() || !Player || !Bush)
		return;
    
	UE_LOG(LogTemp, Warning, TEXT("BushVisibilityManager: Player %s entered Bush"), *Player->GetName());
    
	// Cập nhật map
	PlayerToBushMap.Add(Player, Bush);
    
	// Tính toán lại hiển thị
	RecomputeVisibilityForAll();
}

void ABushVisibilityManager::HandlePlayerExitedBush(AStealthPlayerCharacter* Player, ABushActor* Bush)
{
	// Chỉ chạy trên Server
	if (!HasAuthority() || !Player || !Bush)
		return;
    
	UE_LOG(LogTemp, Warning, TEXT("BushVisibilityManager: Player %s exited Bush"), *Player->GetName());
    
	// Cập nhật map
	PlayerToBushMap.Remove(Player);
    
	// Tính toán lại hiển thị
	RecomputeVisibilityForAll();
}

void ABushVisibilityManager::RecomputeVisibilityForAll()
{
	// Chỉ chạy trên Server
	if (!HasAuthority())
		return;
    
	UE_LOG(LogTemp, Warning, TEXT("BushVisibilityManager: Recomputing visibility for all players"));
    
	// Lấy tất cả Player Character trong thế giới game
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStealthPlayerCharacter::StaticClass(), FoundActors);
    
	// Với mỗi Observer (người nhìn)
	for (AActor* ObserverActor : FoundActors)
	{
		AStealthPlayerCharacter* Observer = Cast<AStealthPlayerCharacter>(ObserverActor);
		if (!Observer)
			continue;
        
		// Với mỗi Target (người bị nhìn)
		for (AActor* TargetActor : FoundActors)
		{
			AStealthPlayerCharacter* Target = Cast<AStealthPlayerCharacter>(TargetActor);
			if (!Target)
				continue;
            
			// Áp dụng quy tắc hiển thị
			ApplyVisibilityRules(Observer, Target);
		}
	}
}

void ABushVisibilityManager::ApplyVisibilityRules(AStealthPlayerCharacter* Observer, AStealthPlayerCharacter* Target)
{
	if (!HasAuthority() || !Observer || !Target)
		return;
    
	bool bShouldBeVisible = ShouldTargetBeVisibleToObserver(Observer, Target);
    
	// Áp dụng trạng thái hiển thị
	Target->SetActorHiddenInGame(!bShouldBeVisible);
    
	UE_LOG(LogTemp, Warning, TEXT("BushVisibilityManager: Player %s %s Player %s"),
		   *Observer->GetName(),
		   bShouldBeVisible ? TEXT("CAN SEE") : TEXT("CANNOT SEE"),
		   *Target->GetName());
}

bool ABushVisibilityManager::ShouldTargetBeVisibleToObserver(AStealthPlayerCharacter* Observer,
	AStealthPlayerCharacter* Target)
{
	// Quy tắc 4: Player luôn thấy chính mình
	if (Observer == Target)
	{
		return true;
	}
    
	// Lấy thông tin bụi cỏ của Observer và Target
	ABushActor* ObserverBush = PlayerToBushMap.FindRef(Observer);
	ABushActor* TargetBush = PlayerToBushMap.FindRef(Target);
    
	// Quy tắc 1: Target trong bụi cỏ, Observer ngoài bụi cỏ -> Ẩn
	if (TargetBush && !ObserverBush)
	{
		return false;
	}
	// Quy tắc 2: Target và Observer trong cùng bụi cỏ -> Hiện
	else if (TargetBush && ObserverBush && TargetBush == ObserverBush)
	{
		return true;
	}
	// Quy tắc 3: Cả hai ngoài bụi cỏ -> Hiện
	else if (!TargetBush && !ObserverBush)
	{
		return true;
	}
    
	// Trường hợp còn lại: Target ngoài bụi cỏ, Observer trong bụi cỏ -> Hiện
	return true;
}


