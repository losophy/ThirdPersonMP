// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ThirdPersonStats.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UThirdPersonStats : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THIRDPERSONMP_API IThirdPersonStats
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Should update kill stat
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Stats")
	void AddKill();

	// Should update death stat
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Stats")
	void AddDeath();

	// Should update scoring stat
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Stats")
	void AddScore(float const InAddedScore);
};
