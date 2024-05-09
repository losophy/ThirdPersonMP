// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonStatsInterface.h"
#include "GameFramework/PlayerState.h"
#include "ThirdPersonPlayerState.generated.h"

/* Common player stats */
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

// Declare delegate for stats changing notification
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatsChangedSignature, FPlayerStats const&, InNewStats);

/**
 * Individual player's stats
 */
UCLASS()
class THIRDPERSONMP_API AThirdPersonPlayerState : public APlayerState, public IThirdPersonStatsInterface
{
	GENERATED_BODY()

#pragma region Stats
public:

	// Override interface methods to interact with the stats
	virtual void AddKill_Implementation() override;
	virtual void AddDeath_Implementation() override;
	virtual void AddScore_Implementation(float const InAddedScore) override;

	// Retrieves current player's stats
	UFUNCTION(BlueprintPure, Category="Stats")
	FORCEINLINE FPlayerStats GetPlayerStats() const noexcept { return Stats; }

	FORCEINLINE FOnStatsChangedSignature& GetOnStatsChanged() noexcept { return OnStatsChanged; }
	
protected:

	/* Actual player's stats. Replicated to all clients */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Stats, Category = "Stats")
	FPlayerStats Stats;

	// Triggered whenever stats are changed.
	UPROPERTY(BlueprintAssignable, Category="Stats")
	FOnStatsChangedSignature OnStatsChanged;

	// Triggered when stats are changed
	UFUNCTION()
	void OnRep_Stats();

	// Override to define replicated variables
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
#pragma endregion
};
