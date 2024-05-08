// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ThirdPersonGameState.generated.h"

/**
 * Extends the base game state to control leaderboard data,
 * such as scores, number of players and on.
 */
UCLASS()
class THIRDPERSONMP_API AThirdPersonGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	
	AThirdPersonGameState();
	
#pragma region Stats
public:

	// Retrieves the number of players in the world
	UFUNCTION(BlueprintPure, Category="TPGameState")
	FORCEINLINE int32 GetPlayerNum() const noexcept { return PlayerArray.Num(); }
	
#pragma endregion
	
};
