// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMainWindow.h"

#include "ThirdPersonGameState.h"
#include "ThirdPersonLeaderboardWidget.h"
#include "Kismet/GameplayStatics.h"

void UThirdPersonMainWindow::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	LeaderboardWidget->SetVisibility(ESlateVisibility::Collapsed);  // Hide by default
}

void UThirdPersonMainWindow::NativeConstruct()
{
	Super::NativeConstruct();

	auto const GameState = UGameplayStatics::GetGameState(this);
	if(auto const LTPGameState =
		Cast<AThirdPersonGameState>(GameState);
		ensureMsgf(LTPGameState, TEXT("Bad game state cast! Given state ref: %s"), *GetNameSafe(GameState)))
	{
		LTPGameState->OnPlayerChangedConnection.AddUObject(this, &UThirdPersonMainWindow::OnPlayerNumChanged);
		SetPlayerNum(LTPGameState->ConnectedPlayers);
		K2_RedrawWidget();
	}
}

void UThirdPersonMainWindow::OnPlayerNumChanged(int32 const NewPlayerNum)
{
	SetPlayerNum(NewPlayerNum);
	K2_RedrawWidget();
}

void UThirdPersonMainWindow::UpdateHealth(float const InNewHealth)
{
	// Clamp the value for showing it as a percentage
	CurrentPlayerHealth = FMath::GetMappedRangeValueClamped(
		FVector2f{ 0.f, 100.f },
		FVector2f{ 0.f, 1.f },
		InNewHealth
	);
}

void UThirdPersonMainWindow::DisplayLeaderboard()
{
	LeaderboardWidget->SetVisibility(ESlateVisibility::Visible);
}

void UThirdPersonMainWindow::HideLeaderboard()
{
	LeaderboardWidget->SetVisibility(ESlateVisibility::Collapsed);
}
