// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonPlayerState.h"

#include "Net/UnrealNetwork.h"

void AThirdPersonPlayerState::AddKill_Implementation()
{
	Stats.KillsCount++;
	OnStatsChanged.Broadcast(Stats);
}

void AThirdPersonPlayerState::AddDeath_Implementation()
{
	Stats.DeathsCount++;
	OnStatsChanged.Broadcast(Stats);
}

void AThirdPersonPlayerState::AddScore_Implementation(float const InAddedScore)
{
	Stats.Score += InAddedScore;
	OnStatsChanged.Broadcast(Stats);
}

void AThirdPersonPlayerState::OnRep_Stats()
{
	OnStatsChanged.Broadcast(Stats);
}

void AThirdPersonPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThirdPersonPlayerState, Stats);
}
