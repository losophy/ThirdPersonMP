// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMainWindow.h"

#include "ThirdPersonLeaderboardWidget.h"
#include "Kismet/GameplayStatics.h"

void UThirdPersonMainWindow::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayersNum = UGameplayStatics::GetNumPlayerStates(this);
	LeaderboardWidget->SetVisibility(ESlateVisibility::Collapsed);  // Hide by default
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
