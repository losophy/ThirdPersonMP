// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonGameState.h"
#include "Net/UnrealNetwork.h"

void AThirdPersonGameState::SetConnectedPlayers(int32 const InNewValue)
{
	ConnectedPlayers = InNewValue;
	OnPlayerChangedConnection.Broadcast(ConnectedPlayers);
}

void AThirdPersonGameState::OnRep_ConnectedPlayers()
{
	OnPlayerChangedConnection.Broadcast(ConnectedPlayers);
}

void AThirdPersonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThirdPersonGameState, ConnectedPlayers);
}
