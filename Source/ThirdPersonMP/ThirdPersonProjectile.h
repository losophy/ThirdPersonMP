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
	
	// Sphere component for testing collision
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class USphereComponent* SphereComponent;

	// Static mesh component for visual representation
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class UStaticMeshComponent* StaticMesh;

	// Projectile movement component for moving this object.
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Explosion effect that is spawned at the location of this projectile destuction
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true), Category = "Components")
	UParticleSystem* ExplosionEffect;
	
#pragma endregion

#pragma region Gameplay
	
protected:

	// User defined damage type to apply to a damaged actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay")
	TSubclassOf<class UDamageType> DamageType;

	// The actual damage dealt to hit actor
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
