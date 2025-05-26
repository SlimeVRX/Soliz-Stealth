// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BushActor.h"
#include "Character/StealthPlayerCharacter.h"
#include "Manager/BushVisibilityManager.h"
#include "GameMode/StealthGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABushActor::ABushActor()
{
	// Cài đặt Actor này được replicate trên mạng
	bReplicates = true;
    
	// Tạo và cài đặt Box Component
	BushOverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BushOverlapVolume"));
	RootComponent = BushOverlapVolume;
    
	// Cấu hình va chạm
	BushOverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BushOverlapVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BushOverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	BushOverlapVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	BushOverlapVolume->SetGenerateOverlapEvents(true);
    
	// Đăng ký các hàm xử lý va chạm
	BushOverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ABushActor::OnOverlapBegin);
	BushOverlapVolume->OnComponentEndOverlap.AddDynamic(this, &ABushActor::OnOverlapEnd);
    
	// Cài đặt kích thước mặc định
	BushOverlapVolume->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
    
	// ID mặc định
	BushID = 0;
}

// Called when the game starts or when spawned
void ABushActor::BeginPlay()
{
	Super::BeginPlay();

	// Log thông tin khi spawn
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("ABushActor spawned on Server! ID: %d"), BushID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABushActor spawned on Client! ID: %d"), BushID);
	}
	
}

void ABushActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Chỉ xử lý trên Server
	if (HasAuthority())
	{
		// Kiểm tra xem Actor va chạm có phải là ACustomPlayerCharacter không
		AStealthPlayerCharacter* PlayerCharacter = Cast<AStealthPlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: Player %s entered Bush %d"), 
				   *PlayerCharacter->GetName(), BushID);
            
			// Lấy BushVisibilityManager từ GameMode
			AStealthGameMode* GameMode = Cast<AStealthGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode)
			{
				ABushVisibilityManager* Manager = GameMode->GetBushVisibilityManager();
				if (Manager)
				{
					Manager->HandlePlayerEnteredBush(PlayerCharacter, this);
                    
					// Thông báo cho client về sự kiện này
					Multicast_NotifyBushOverlap(PlayerCharacter, true);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("BushVisibilityManager not found!"));
				}
			}
		}
	}
}

void ABushActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// Chỉ xử lý trên Server
	if (HasAuthority())
	{
		// Kiểm tra xem Actor va chạm có phải là ACustomPlayerCharacter không
		AStealthPlayerCharacter* PlayerCharacter = Cast<AStealthPlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: Player %s exited Bush %d"), 
				   *PlayerCharacter->GetName(), BushID);
            
			// Lấy BushVisibilityManager từ GameMode
			AStealthGameMode* GameMode = Cast<AStealthGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode)
			{
				ABushVisibilityManager* Manager = GameMode->GetBushVisibilityManager();
				if (Manager)
				{
					Manager->HandlePlayerExitedBush(PlayerCharacter, this);
                    
					// Thông báo cho client về sự kiện này
					Multicast_NotifyBushOverlap(PlayerCharacter, false);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("BushVisibilityManager not found!"));
				}
			}
		}
	}
}

void ABushActor::Multicast_NotifyBushOverlap_Implementation(AStealthPlayerCharacter* Player, bool bEntered)
{
	// Chỉ xử lý trên client sở hữu nhân vật
	if (Player && Player->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client: Player %s %s Bush %d"), 
			   *Player->GetName(), bEntered ? TEXT("entered") : TEXT("exited"), BushID);
               
		// Thêm logic client-side ở đây nếu cần
		// Ví dụ: hiệu ứng âm thanh, hình ảnh, UI, v.v.
	}
}
