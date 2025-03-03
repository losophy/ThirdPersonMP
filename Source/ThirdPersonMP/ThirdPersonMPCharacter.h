// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonStatsInterface.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ThirdPersonMPCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDiedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHealthUpdateSignature, float const, InUpdatedHealth);

UCLASS(config=Game)
class AThirdPersonMPCharacter : public ACharacter
{
	GENERATED_BODY()
	
#pragma region Components
	
	/** 摄像机吊杆将摄像机置于角色身后 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** 跟随摄像机 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
#pragma endregion

#pragma region Input
protected:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// Fire input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	
protected:
	
	// APawn接口
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
#pragma endregion
	
public:
	
	AThirdPersonMPCharacter();
	
protected:
	
#pragma region Gameplay

#pragma region Visual
protected:

	// Material instance to pain the character's mesh in, when he is dead
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Visual")
	TSoftObjectPtr<UMaterialInstance> DeathMaterialInstance;

	// Paints each client's mesh to the death material instance when killed. Must be called cautiously
	UFUNCTION(NetMulticast, Reliable)
	void PaintMesh();

	// Handler for asynchronous soft object reference loading
	void OnMaterialInstanceLoad();
	
#pragma endregion
	
#pragma region Health

	// 玩家的最大生命值。这是玩家的最高生命值。此值为玩家出生时的生命值。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Health")
	float MaxHealth;

	// 玩家的当前生命值。降到0就表示死亡。
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category="Character|Health")
	bool bDead;

	// Triggered when player's health is updated.
	UPROPERTY(BlueprintAssignable, Category="Character|Health")
	FOnPlayerHealthUpdateSignature OnPlayerHealthUpdate;
	
public:

	// 当前生命值的存值函数。将此值的范围限定在0到MaxHealth之间，并调用OnHealthUpdate。仅在服务器上调用。
	UFUNCTION(BlueprintCallable, Category="Character|Health")
	void SetCurrentHealth(float const InHealthValue) noexcept;

	// 最大生命值的取值函数
	UFUNCTION(BlueprintPure, Category="Character|Health")
	FORCEINLINE float GetMaxHealth() const noexcept
	{ return MaxHealth; }
	
	// 当前生命值的取值函数
	UFUNCTION(BlueprintPure, Category="Character|Health")
	FORCEINLINE float GetCurrentHealth() const noexcept
	{ return CurrentHealth; }

	// RepNotify，用于同步对当前生命值所做的更改。
	UFUNCTION()
	void OnRep_CurrentHealth();
	
	FORCEINLINE FOnPlayerHealthUpdateSignature& GetOnPlayerHealthUpdate() noexcept
	{ return OnPlayerHealthUpdate; }

protected:

	// 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify
	void OnHealthUpdate() noexcept;

	// Called on the server and all other updates stats are replicated to clients
	void OnDeathUpdate();

#pragma endregion
	
#pragma region Events

	// The last actor that caused damage to this character
	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category="Damage")
	AController* MyLastDamageInstigator;
	
	/** 承受伤害的事件。从APawn覆盖。*/
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
#pragma endregion

#pragma region Fire

protected:

	// When firing, will spawn a projectile of this class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Fire")
	TSubclassOf<class AThirdPersonProjectile> ProjectileClass;
	
	// 射击之间的延迟，单位为秒。用于控制测试发射物的射击速度，还可防止服务器函数的溢出导致将SpawnProjectile直接绑定至输入。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Fire")
	float FireRate;

	// 若为true，则正在发射投射物。
	bool bFiringWeapon;

	// 定时器句柄，用于提供生成间隔时间内的射速延迟。
	FTimerHandle FiringTimer;

	//@Note: StartFire and StopFire are protections measures from frequent RPC calls
	
	// 用于启动武器发射的函数。应仅可由本地玩家触发。
	UFUNCTION(BlueprintCallable, Category="Character|Fire")
	void StartFire();

	// 用于结束武器射击的函数。一旦调用这段代码，玩家可再次使用StartFire。
	UFUNCTION(BlueprintCallable,  Category="Character|Fire")
	void StopFire();

	// 用于生成投射物的服务器函数。本项目中实现的唯一RPC
	UFUNCTION(Server, Reliable)
	void HandleFire();
	
#pragma endregion

#pragma region Destruction
public:

	FORCEINLINE FOnPlayerDiedSignature& GetOnPlayerDied() noexcept { return OnPlayerDied; }
	
protected:

	// Triggered whenever character's health reaches 0.
	UPROPERTY(BlueprintAssignable, Category="Character|Destruction")
	FOnPlayerDiedSignature OnPlayerDied;

	// The amount of time before the actor will be destroyed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Destruction")
	float DestructionTime;
	
	// Timer for the destruction delay
	FTimerHandle DestroyDelayTimer;

	// Destroys this pawn and notifies other bound objects about it.
	// Also simulates physics for local mesh
	// Should be called only on the server
	UFUNCTION(BlueprintCallable, Category="Character|Destruction")
	void PlayerDie();

#pragma endregion

	
	// To add mapping context
	virtual void BeginPlay() override;
	
#pragma endregion
	
protected:
	
	// 属性复制
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** 返回CameraBoom子对象 **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
public:
};

