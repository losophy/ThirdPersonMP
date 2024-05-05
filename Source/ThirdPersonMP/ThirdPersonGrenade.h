// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonProjectile.h"
#include "ThirdPersonGrenade.generated.h"

/**
 * Extends functionality of AThirdPersonProjectile class
 * and behaves like a thrown grenade with a delayed explosion
 */
UCLASS()
class THIRDPERSONMP_API AThirdPersonGrenade : public AThirdPersonProjectile
{
	GENERATED_BODY()
public:
	AThirdPersonGrenade();

#pragma region Gameplay
	
protected:
	
	// Specific damage that is caused to hit actors. Intended to be lower than the actual damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float HitDamage;
	
	// The amount of time this projectile takes to explode in seconds
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float ExplosionTime;

	// Timer that countdowns explosion
	FTimerHandle ExplosionTimer;

	// Override begin play to setup a timer for explosion
	virtual void BeginPlay() override;

	// Perform actual damage causing and destroy this projectile
	void OnExplosionTimerExpire();
	
#pragma endregion
	
#pragma region Events

	// Override projectile impact, not to destroy itself after hit
	virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	                                const FHitResult& Hit) override;
	
#pragma endregion
	
};
