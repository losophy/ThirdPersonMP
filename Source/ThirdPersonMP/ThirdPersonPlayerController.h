// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ThirdPersonControllerInterface.h"
#include "ThirdPersonPlayerController.generated.h"

/**
 * Implements ThirdPersonControllerInterface and realises
 * respawning logic
 */
UCLASS()
class THIRDPERSONMP_API AThirdPersonPlayerController : public APlayerController, public IThirdPersonControllerInterface
{
	GENERATED_BODY()

#pragma region Gameplay

#pragma region Respawn
	
public:
	//@note: All the functions should be ensured to call on the server side.
	//The binding is ensured to be only on the server

	virtual void OnPossess(APawn* InPawn) override;
	
	// Redirects respawn request to the game mode when character was dead
	virtual void RespawnPlayer_Implementation() override;
	
protected:
	// Triggeted whenever controlled pawn is dead
	UFUNCTION()
	void OnControlledPawnDie();
	
#pragma endregion
	
#pragma endregion
};
