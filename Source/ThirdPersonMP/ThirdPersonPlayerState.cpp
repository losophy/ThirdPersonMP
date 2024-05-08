// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonPlayerState.h"

#include "Net/UnrealNetwork.h"

void AThirdPersonPlayerState::AddKill_Implementation()
{
	Stats.KillsCount++;
}

void AThirdPersonPlayerState::AddDeath_Implementation()
{
	Stats.DeathsCount++;
}

void AThirdPersonPlayerState::AddScore_Implementation(float const InAddedScore)
{
	Stats.Score += InAddedScore;
}

void AThirdPersonPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThirdPersonPlayerState, Stats);
}
