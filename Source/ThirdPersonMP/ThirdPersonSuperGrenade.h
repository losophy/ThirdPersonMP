// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonGrenade.h"
#include "ThirdPersonSuperGrenade.generated.h"

/**
 * Extends AThirdPersonGrenade to behave like
 * a thrown grenade which after landing, throws
 * three more grenade from its center
 */
UCLASS()
class THIRDPERSONMP_API AThirdPersonSuperGrenade : public AThirdPersonGrenade
{
	GENERATED_BODY()

public:
	AThirdPersonSuperGrenade();
	
#pragma region Gameplay

protected:
	
	/* Grenade class to spawn after hit. Be cautious when choosing the **AThirdPersonSuperGrenade**
	 * or its childs as classes as they produce a lot of RPC calls based on the amount of grenades.
	 * You can set bProtectedGrenadeSequence to true and set the number of times you want
	 * your grenades to duplicate.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay|Grenade")
	TSubclassOf<AThirdPersonGrenade> GrenadeClass;

	// The radius in which to spawn grenades
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay|Grenade")
	float GrenadesSpawnRadius;
	
	// The number of additional grenades to be spawn from the center
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay|Grenade")
	int32 NumGrenades;
	
	// Flag to control that grenades were spawned, to avoid additional spawning.
	bool bGrenadesSpawned;

	// Spawns more grenades in a circular pattern from the center.
	// Should be called with caution avoiding frequent use.
	UFUNCTION(Server, Reliable)
	void SpawnGrenades();

#pragma endregion

protected:
#pragma region Events

	virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	                                const FHitResult& Hit) override;
	
#pragma endregion
};
