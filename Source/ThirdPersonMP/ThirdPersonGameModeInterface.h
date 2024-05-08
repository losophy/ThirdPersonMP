// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ThirdPersonGameModeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UThirdPersonGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THIRDPERSONMP_API IThirdPersonGameModeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Handles the killing of a player by another player
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="TPGameMode")
	void HandlePlayerKilled(AController* InKiller, AController* InVictim);
};
