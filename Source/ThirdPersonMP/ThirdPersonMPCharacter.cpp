// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonMPCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ThirdPersonGameModeInterface.h"
#include "ThirdPersonProjectile.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AThirdPersonMPCharacter

AThirdPersonMPCharacter::AThirdPersonMPCharacter()
{
	// 设置碰撞胶囊体的大小
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// 控制器旋转时不旋转。只影响摄像机。
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 配置角色移动
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// 注意：为了提高迭代速度，可在角色蓝图中微调这些及其他许多变量，
	// 而不是通过重新编译来调整它们
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // 摄像机以这个距离跟在角色身后
	CameraBoom->bUsePawnControlRotation = true; // 基于控制器旋转吊臂

	// 创建跟随摄像头
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 将摄像机连接到吊杆末端，调节吊杆以匹配控制器方向
	FollowCamera->bUsePawnControlRotation = false; // 摄像机不相对于吊臂旋转

	// 注意：骨架网格体和网格体组件上的动画蓝图引用（继承自角色）
	// 都在名为MyCharacter的派生蓝图资产中设置（以避免C++环境下的直接内容引用）

	//初始化玩家生命值
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;

	MyLastDamageInstigator = nullptr;

	// 初始化射速
	FireRate = 0.25f;
	bFiringWeapon = false;
	//初始化投射物类
	ProjectileClass = AThirdPersonProjectile::StaticClass();

	// Initialize destruction vars
	DestructionTime = 3.6f;
	bDead = false;
}

void AThirdPersonMPCharacter::OnMaterialInstanceLoad()
{
	if(ensureMsgf(DeathMaterialInstance, TEXT("Bad material instance load!")))
	{
		// Set all materials to the death instance
		for (int32 i = 0; i < GetMesh()->GetMaterials().Num(); ++i)
		{
			GetMesh()->SetMaterial(i, DeathMaterialInstance.Get());
		}
	}
}

void AThirdPersonMPCharacter::PaintMesh_Implementation()
{
	// Asynchronously load material instance if not valid
	if(!DeathMaterialInstance.IsValid())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			DeathMaterialInstance.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &AThirdPersonMPCharacter::OnMaterialInstanceLoad)
		);
	}
	else
	{
		OnMaterialInstanceLoad();
	}
}

void AThirdPersonMPCharacter::SetCurrentHealth(float const InHealthValue) noexcept
{
	if(GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(InHealthValue, 0.f, MaxHealth);
		OnHealthUpdate();  // Call on health update because rep notify does not notify the actual server itself
	}
}

void AThirdPersonMPCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AThirdPersonMPCharacter::OnDeathUpdate()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		auto const GameMode = UGameplayStatics::GetGameMode(this);
		IThirdPersonGameModeInterface::Execute_HandlePlayerKilled(GameMode,
				MyLastDamageInstigator,
				GetController()
			);
	}
}

void AThirdPersonMPCharacter::OnHealthUpdate() noexcept
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("My role is %s"), GetLocalRole() == ROLE_Authority ? TEXT("Authority") : TEXT("Client")));
	
	// 客户端特定的功能
	if(IsLocallyControlled())
	{
		FString const LHealthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LHealthMessage);

		if(CurrentHealth <= 0 && !bDead)
		{
			FString const LDeathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LDeathMessage);
		}
	}

	// 服务器特定的功能
	if(GetLocalRole() == ROLE_Authority)
	{
		FString const LHealthMessage = FString::Printf(TEXT("%s now have %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LHealthMessage);

		// Utilize the opprotunity of being on the server, and restart the player right away
		if(CurrentHealth <= 0 && !bDead)
		{
			// While restarting. Destroy this pawn.
			PlayerDie();
		}
	}

	//在所有机器上都执行的函数。
		/*
			因任何因伤害或死亡而产生的特殊功能都应放在这里。
		*/

	// Broadcast health update both locally and on the server
	OnPlayerHealthUpdate.Broadcast(CurrentHealth);
}

float AThirdPersonMPCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	MyLastDamageInstigator = EventInstigator;
	float const LAppliedDamage = CurrentHealth - DamageAmount;
	SetCurrentHealth(LAppliedDamage);
	return LAppliedDamage;
}

void AThirdPersonMPCharacter::StartFire()
{
	if(!bFiringWeapon)
	{
		bFiringWeapon = true;
		GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &AThirdPersonMPCharacter::StopFire,
			FireRate, false);
		
		HandleFire();
	}
}

void AThirdPersonMPCharacter::StopFire()
{
	bFiringWeapon = false;
}

//@ note: Ensured to be called on server
void AThirdPersonMPCharacter::HandleFire_Implementation()
{
	// Spawn in front of the player, a little bit higher.
	FVector const LSpawnLocation = GetActorLocation() +
		(GetActorRotation().Vector() * 100.f) + (GetActorUpVector() * 50.f);
	FRotator const LSpawnRotation = GetActorRotation();

	FActorSpawnParameters LSpawnParameters;
	LSpawnParameters.Instigator = GetInstigator();
	LSpawnParameters.Owner = this;

	// Spawn actor at the server
	GetWorld()->SpawnActor<AThirdPersonProjectile>(ProjectileClass, LSpawnLocation, LSpawnRotation, LSpawnParameters);
}

void AThirdPersonMPCharacter::PlayerDie()
{
	// Ensure to call only on the server
	if(ensureMsgf(GetLocalRole() == ROLE_Authority, TEXT("Bad function call. %s can be called only on the server!"),
		*FString(__FUNCTION__)))
	{
		// Set state of the player as dead
		bDead = true;
		OnDeathUpdate();
		
		// Paint mesh on all clients
		PaintMesh();

		GetWorld()->GetTimerManager().SetTimer(DestroyDelayTimer, [this]
		{
			
			// Notify anyone who was expecting the player to be dead
			OnPlayerDied.Broadcast();
			Destroy();
		
		}, DestructionTime, false);
		
	}
}

void AThirdPersonMPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonMPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 设置游戏进程键绑定
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonMPCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonMPCharacter::Look);

		// 处理发射投射物
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AThirdPersonMPCharacter::StartFire);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// 复制的属性
void AThirdPersonMPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//复制当前生命值。
	DOREPLIFETIME(AThirdPersonMPCharacter, CurrentHealth);
	DOREPLIFETIME(AThirdPersonMPCharacter, MyLastDamageInstigator);
}

void AThirdPersonMPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AThirdPersonMPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
