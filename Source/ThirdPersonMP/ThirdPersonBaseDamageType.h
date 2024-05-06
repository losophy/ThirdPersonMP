// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ThirdPersonBaseDamageType.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONMP_API UThirdPersonBaseDamageType : public UDamageType
{
	GENERATED_BODY()
protected:

	// Optinal damage name that is going to be displayed at a player's screen when he is
	// killed indicating the type of damage he was killed from
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay|Damage")
	FString DamageName;
	
};
