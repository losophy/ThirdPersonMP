// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ThirdPersonLeaderboardWidget.generated.h"


class AThirdPersonPlayerState;

/* Subclass for UThirdPersonLeaderboardWidget that
 * realises an entry for the list view
 */
UCLASS(Blueprintable)
class UThirdPersonLeaderboardElement : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:

	// Internal pointer to player state from which we get stats data info
	UPROPERTY(BlueprintReadOnly, Transient, Category="Stats")
	AThirdPersonPlayerState* PlayerState = nullptr;

	// Triggered whenever attached player state's stats were updated
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Stats Updated"), Category="Stats")
	void K2_OnStatsChanged(struct FPlayerStats const& InChangedStats);
	
	// Override to initialize data
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
};

/**
 *  Widget that displays leaderboard.
 *  Displays all players in the world as well
 *  as their stats
 */
UCLASS()
class THIRDPERSONMP_API UThirdPersonLeaderboardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	// List of all players in the world
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget = true), Category = "Widgets")
	class UListView* PlayersList;

	// Override to initialize the players list
	virtual void NativeConstruct() override;
};
