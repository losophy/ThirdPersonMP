// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ThirdPersonLeaderboardWidget.generated.h"


/* Subclass for UThirdPersonLeaderboardWidget that
 * realises an entry for the list view
 */
UCLASS(Blueprintable)
class UThirdPersonLeaderboardElement : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category="Data")
	class UThirdPersonStatsComponent* PlayerStatComponent = nullptr;
	
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
