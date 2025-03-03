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
	
	/** ��������˽���������ڽ�ɫ��� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** ��������� */
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
	
	// APawn�ӿ�
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

	// ��ҵ��������ֵ��������ҵ��������ֵ����ֵΪ��ҳ���ʱ������ֵ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Health")
	float MaxHealth;

	// ��ҵĵ�ǰ����ֵ������0�ͱ�ʾ������
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category="Character|Health")
	bool bDead;

	// Triggered when player's health is updated.
	UPROPERTY(BlueprintAssignable, Category="Character|Health")
	FOnPlayerHealthUpdateSignature OnPlayerHealthUpdate;
	
public:

	// ��ǰ����ֵ�Ĵ�ֵ����������ֵ�ķ�Χ�޶���0��MaxHealth֮�䣬������OnHealthUpdate�����ڷ������ϵ��á�
	UFUNCTION(BlueprintCallable, Category="Character|Health")
	void SetCurrentHealth(float const InHealthValue) noexcept;

	// �������ֵ��ȡֵ����
	UFUNCTION(BlueprintPure, Category="Character|Health")
	FORCEINLINE float GetMaxHealth() const noexcept
	{ return MaxHealth; }
	
	// ��ǰ����ֵ��ȡֵ����
	UFUNCTION(BlueprintPure, Category="Character|Health")
	FORCEINLINE float GetCurrentHealth() const noexcept
	{ return CurrentHealth; }

	// RepNotify������ͬ���Ե�ǰ����ֵ�����ĸ��ġ�
	UFUNCTION()
	void OnRep_CurrentHealth();
	
	FORCEINLINE FOnPlayerHealthUpdateSignature& GetOnPlayerHealthUpdate() noexcept
	{ return OnPlayerHealthUpdate; }

protected:

	// ��ӦҪ���µ�����ֵ���޸ĺ������ڷ������ϵ��ã����ڿͻ����ϵ�������ӦRepNotify
	void OnHealthUpdate() noexcept;

	// Called on the server and all other updates stats are replicated to clients
	void OnDeathUpdate();

#pragma endregion
	
#pragma region Events

	// The last actor that caused damage to this character
	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category="Damage")
	AController* MyLastDamageInstigator;
	
	/** �����˺����¼�����APawn���ǡ�*/
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
#pragma endregion

#pragma region Fire

protected:

	// When firing, will spawn a projectile of this class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Fire")
	TSubclassOf<class AThirdPersonProjectile> ProjectileClass;
	
	// ���֮����ӳ٣���λΪ�롣���ڿ��Ʋ��Է����������ٶȣ����ɷ�ֹ������������������½�SpawnProjectileֱ�Ӱ������롣
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Fire")
	float FireRate;

	// ��Ϊtrue�������ڷ���Ͷ���
	bool bFiringWeapon;

	// ��ʱ������������ṩ���ɼ��ʱ���ڵ������ӳ١�
	FTimerHandle FiringTimer;

	//@Note: StartFire and StopFire are protections measures from frequent RPC calls
	
	// ����������������ĺ�����Ӧ�����ɱ�����Ҵ�����
	UFUNCTION(BlueprintCallable, Category="Character|Fire")
	void StartFire();

	// ���ڽ�����������ĺ�����һ��������δ��룬��ҿ��ٴ�ʹ��StartFire��
	UFUNCTION(BlueprintCallable,  Category="Character|Fire")
	void StopFire();

	// ��������Ͷ����ķ���������������Ŀ��ʵ�ֵ�ΨһRPC
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
	
	// ���Ը���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** ����CameraBoom�Ӷ��� **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
public:
};

