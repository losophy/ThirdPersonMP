// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ThirdPersonGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnConnectedPlayersChanged, int32 const);


/**
 * Displays the number of connected players
 */
UCLASS()
class THIRDPERSONMP_API AThirdPersonGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	// The amount of connected players in the game. Replicates to all clients
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ConnectedPlayers, Category="Network")
	int32 ConnectedPlayers;
	
	// Triggered when a new player connects, or the existing player logs out. Can be bound both on the server and the client
	FOnConnectedPlayersChanged OnPlayerChangedConnection;

	// Sets new value for connected players
	UFUNCTION(BlueprintCallable, Category="Network")
	void SetConnectedPlayers(int32 const InNewValue);

protected:
	
	// Triggered when connected players change is replicated
	UFUNCTION()
	void OnRep_ConnectedPlayers();
	
	// Override to define replicated properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
