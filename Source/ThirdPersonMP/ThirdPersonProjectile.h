// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThirdPersonProjectile.generated.h"

UCLASS()
class THIRDPERSONMP_API AThirdPersonProjectile : public AActor
{
	GENERATED_BODY()
	
#pragma region Components

protected:
	
	// ���ڲ�����ײ�����������
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class USphereComponent* SphereComponent;

	// �����ṩ�����Ӿ�����Ч���ľ�̬�����塣
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class UStaticMeshComponent* StaticMesh;

	// ���ڴ���Ͷ�����ƶ����ƶ������
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// ��Ͷ����ײ���������󲢱�ըʱʹ�õ����ӡ�
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	UParticleSystem* ExplosionEffect;
	
#pragma endregion

#pragma region Gameplay
	
protected:

	// ��Ͷ���ｫ��ɵ��˺����ͺ��˺���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay")
	TSubclassOf<class UDamageType> DamageType;

	// ��Ͷ������ɵ��˺���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay")
	float Damage;

	// Radius of this projectile explosion after hit
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay")
	float ExplosionRadius;

	// Object types that are exposed to the explosion damage of this projectile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay")
	TArray<TEnumAsByte<EObjectTypeQuery>> DamageableTypes;

	/*Function to raycast nearest actors and cause damage to them.
	 * @AtLocation - if empty, explodes at this projectile's location
	 */
	void Explode(FVector AtLocation = {});

	
#pragma endregion
	
public:
	
	// Sets default values for this actor's properties
	AThirdPersonProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

#pragma region Events

	// Triggered whenever SphereComponent hits anything
	UFUNCTION()
	virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
#pragma endregion

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
