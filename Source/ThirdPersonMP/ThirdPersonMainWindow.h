// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThirdPersonMainWindow.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONMP_API UThirdPersonMainWindow : public UUserWidget
{
	GENERATED_BODY()

protected:

	// Override to initialized players num
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	// Implementable event for handling widget redrawing
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "Redraw"), Category="UI")
	void K2_RedrawWidget();

#pragma region Network

	// Triggered when the number of players changed in the game
	void OnPlayerNumChanged(int32 const NewPlayerNum);
	
#pragma endregion
	
#pragma region Health
public:

	// Updates health variable for this widget
	UFUNCTION(BlueprintCallable, Category="UI|Health")
	void UpdateHealth(float const InNewHealth);
	
protected:

	// Currently replicated player health
	UPROPERTY(BlueprintReadOnly, meta=(ClampMin = 0.f, ClampMax = 1.f), Category="UI|Health")
	float CurrentPlayerHealth = 1.f;
	
#pragma endregion

#pragma region Stats
	
protected:
	// The amount of players currently in the
	// world that is displayed locally for each user
	UPROPERTY(BlueprintReadOnly, Category="UI|Stats")
	int32 PlayerNum = 1;

	// The actual widget of leaderboard that is displayed at the center for a player
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta=(BindWidget = true), Category = "UI")
	class UThirdPersonLeaderboardWidget* LeaderboardWidget;
	
public:
	// Shows the leaderboard widget on the screen
	UFUNCTION(BlueprintCallable, Category="UI|Stats")
	void DisplayLeaderboard();

	// Hides leaderboard widget
	UFUNCTION(BlueprintCallable, Category="UI|Stats")
	void HideLeaderboard();

	// Sets players num variable
	FORCEINLINE void SetPlayerNum(int32 const InNewPlayerNum) { PlayerNum = InNewPlayerNum; }
	
#pragma endregion
};
