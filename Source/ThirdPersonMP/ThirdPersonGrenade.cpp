// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonGrenade.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AThirdPersonGrenade::AThirdPersonGrenade()
{
	//@note: This class is already replicated

	// Setup projectile movement with a lower speed and a gravity affection
	ProjectileMovementComponent->InitialSpeed = 850.f;
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	
	// Let the grenade bounce from surface
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Friction = 0.55f;

	// Increase damage of this projectile
	Damage = 45.f;
	HitDamage = 4.f;

	// Setup explosion time
	ExplosionTime = 5.f;
}

void AThirdPersonGrenade::BeginPlay()
{
	Super::BeginPlay();

	// Setup timer to countdown the explosion
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimer, this, &AThirdPersonGrenade::OnExplosionTimerExpire,
		ExplosionTime, false);
}

void AThirdPersonGrenade::OnExplosionTimerExpire()
{
	// Cause damage to nearest actors
	Explode();

	// Destroy projectile and do visual effects
	Destroy();
}

void AThirdPersonGrenade::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Hit actor with the low hit damage
	if(OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, HitDamage, NormalImpulse, Hit,
			GetInstigator()->Controller, this, DamageType);
	}

	// We don't destroy this projectile because it has custom timer for explosion
}
