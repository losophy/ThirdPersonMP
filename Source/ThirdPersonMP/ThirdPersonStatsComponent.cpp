// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonStatsComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UThirdPersonStatsComponent::UThirdPersonStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	// Enable replication
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UThirdPersonStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UThirdPersonStatsComponent::AddKillsCount(int32 const InAddCount)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		Stats.KillsCount += InAddCount;
	}
}

void UThirdPersonStatsComponent::AddDeathsCount(int32 const InAddCount)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		Stats.DeathsCount += InAddCount;
	}
}

void UThirdPersonStatsComponent::AddScore(float const InAddScore)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		Stats.Score += InAddScore;
	}
}

void UThirdPersonStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UThirdPersonStatsComponent, Stats);
}