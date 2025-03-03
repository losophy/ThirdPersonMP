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
	
	// 用于测试碰撞的球体组件。
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class USphereComponent* SphereComponent;

	// 用于提供对象视觉呈现效果的静态网格体。
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class UStaticMeshComponent* StaticMesh;

	// 用于处理投射物移动的移动组件。
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// 在投射物撞击其他对象并爆炸时使用的粒子。
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	UParticleSystem* ExplosionEffect;
	
#pragma endregion

#pragma region Gameplay
	
protected:

	// 此投射物将造成的伤害类型和伤害。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay")
	TSubclassOf<class UDamageType> DamageType;

	// 此投射物造成的伤害。
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
