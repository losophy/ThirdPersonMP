// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThirdPersonStatsComponent.generated.h"

USTRUCT(Blueprintable)
struct FPlayerStats
{
	GENERATED_BODY()

	// The amount of kills for the palyer
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player|Stats")
	int32 KillsCount = 0;

	// The amount of deaths player had
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player|Stats")
	int32 DeathsCount = 0;

	// Player's score
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player|Stats")
	float Score = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONMP_API UThirdPersonStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UThirdPersonStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#pragma region Stats

	/*@note: These functions and the stats variable should be
	 * ensured to be called only on the server, and replicate to
	 * the clients since we don't want players to have local
	 * stats
	 */
	
	// Currently held player's stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Player|Stats")
	FPlayerStats Stats;

	// Adds a number to the kills count in stats. By default, adds one kill
	UFUNCTION(BlueprintCallable, Category = "Player|Stats")
	void AddKillsCount(int32 const InAddCount = 1);

	// Adds a number of deaths for this character. By default, adds one death
	UFUNCTION(BlueprintCallable, Category = "Player|Stats")
	void AddDeathsCount(int32 const InAddCount = 1);

	// Adds score to this player
	UFUNCTION(BlueprintCallable, Category = "Player|Stats")
	void AddScore(float const InAddScore);

#pragma endregion

	// Override to add replicated properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
