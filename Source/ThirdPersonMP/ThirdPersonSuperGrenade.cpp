// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonSuperGrenade.h"

AThirdPersonSuperGrenade::AThirdPersonSuperGrenade()
{
	NumGrenades = 3;
	GrenadesSpawnRadius = 100.f;
	bGrenadesSpawned = false;

	// Setup grenade gameplay defaults
	GrenadeClass = AThirdPersonGrenade::StaticClass();
	GrenadesSpawnRadius = 100.f;
	NumGrenades = 3;
	bGrenadesSpawned = false;
}

void AThirdPersonSuperGrenade::SpawnGrenades_Implementation()
{
	// Iterate over the number of grenades and throw each at its specific angle
	for (int32 i = 0; i < NumGrenades; ++i)
	{
		float const LAngle = (360.f / NumGrenades) * i;
		FVector const LDirection = FRotator{50.f, LAngle, 0.f}.Vector().GetSafeNormal();
		FVector const LSpawnLocation = GetActorLocation() + LDirection * GrenadesSpawnRadius;

		FActorSpawnParameters LSpawnParameters;
		LSpawnParameters.Instigator = GetInstigator();
		LSpawnParameters.Owner = this;
		
		// Spawn the grenade at the server
		if(ensureAlwaysMsgf(!GrenadeClass->IsChildOf(AThirdPersonSuperGrenade::StaticClass()), TEXT("Bad class! You shouldn't use"
																							  "Super grenade as the additional throwing class")))
			
			GetWorld()->SpawnActor<AThirdPersonGrenade>(GrenadeClass, LSpawnLocation, LDirection.Rotation(), LSpawnParameters);
	}
}

void AThirdPersonSuperGrenade::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Proceed with the base code
	Super::OnProjectileImpact(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	
	if(!bGrenadesSpawned)
	{
		bGrenadesSpawned = true;
		SpawnGrenades();
	}
}
