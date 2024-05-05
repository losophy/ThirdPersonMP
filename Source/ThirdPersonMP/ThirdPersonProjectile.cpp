// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


// Sets default values
AThirdPersonProjectile::AThirdPersonProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Enable replication
	bReplicates = true;

	// Setup sphere component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(37.5f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SphereComponent->OnComponentHit.AddDynamic(this, &AThirdPersonProjectile::OnProjectileImpact);
	RootComponent = SphereComponent;

	// Setup static mesh component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	if(DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector{0.f, 0.f, -40.f});
		StaticMesh->SetRelativeScale3D(FVector{0.75f, 0.75f, 0.75f}); // Should match the SphereComponent's size
	}

	// Setup Projectile movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1500.f;
	ProjectileMovementComponent->MaxSpeed = 1500.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	// Setup explosion effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));

	if(DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	// Setup damage
	DamageType = UDamageType::StaticClass();
	Damage = 10.f;
	ExplosionRadius = 300.f;
	DamageableTypes = {};
}

// Called when the game starts or when spawned
void AThirdPersonProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AThirdPersonProjectile::Destroyed()
{
	Super::Destroyed();

	// Spawn explosion effect for destruction imitation
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(), ExplosionEffect, GetActorLocation(),
		FRotator::ZeroRotator, FVector{1.f, 1.f, 1.f},
		true, EPSCPoolMethod::AutoRelease
	);
}

void AThirdPersonProjectile::Explode(FVector AtLocation)
{
	// Setup location to be always valid, unless explicitly set by user
	AtLocation = AtLocation.IsNearlyZero() ? GetActorLocation() : AtLocation;
	
	TArray<AActor*> LOtherHitActors;
	UKismetSystemLibrary::SphereOverlapActors(this, AtLocation,
	                                          ExplosionRadius, DamageableTypes, nullptr, {}, LOtherHitActors);

	for (auto LHitActor : LOtherHitActors)
	{
		FVector const LHitDirection = (AtLocation - LHitActor->GetActorLocation()).GetSafeNormal();
		UGameplayStatics::ApplyPointDamage(LHitActor, Damage, LHitDirection, {},
		                                   GetInstigator()->Controller, this, DamageType);
	}
}

void AThirdPersonProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode(Hit.Location);
	Destroy();
}

// Called every frame
void AThirdPersonProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

