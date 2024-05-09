// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonGameModeInterface.h"
#include "GameFramework/GameModeBase.h"
#include "ThirdPersonMPGameMode.generated.h"


UCLASS(minimalapi)
class AThirdPersonMPGameMode : public AGameModeBase, public IThirdPersonGameModeInterface
{
	GENERATED_BODY()

public:
	AThirdPersonMPGameMode();

#pragma region Gameplay

#pragma region Respawn
	
	virtual void RestartPlayer(AController* NewPlayer) override;
	
#pragma endregion

#pragma region Stats

	// Handles killing of a player by another player
	virtual void HandlePlayerKilled_Implementation(AController* InKiller, AController* InVictim) override;
	
#pragma endregion
	
#pragma endregion

#pragma region Network
protected:
	
	// Override to increment the connected players
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Override to decrement the connected players
	virtual void Logout(AController* Exiting) override;
#pragma endregion
};



