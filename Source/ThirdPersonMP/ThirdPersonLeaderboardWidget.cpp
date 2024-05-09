// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonLeaderboardWidget.h"

#include "ThirdPersonGameState.h"
#include "ThirdPersonPlayerState.h"
#include "Components/ListView.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void UThirdPersonLeaderboardElement::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (auto const AsPlayerState = Cast<AThirdPersonPlayerState>(ListItemObject);
		ensureMsgf(AsPlayerState, TEXT("Bad user object cast!")))
	{
		PlayerState = AsPlayerState;
		PlayerState->GetOnStatsChanged().AddUniqueDynamic(this, &UThirdPersonLeaderboardElement::K2_OnStatsChanged);
	}

	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}

void UThirdPersonLeaderboardWidget::OnPlayerStatesChange()
{
	// Redraw newly added or removed player states
	auto const LGameState = UGameplayStatics::GetGameState(this);
	if(auto const LTPGameState = Cast<AThirdPersonGameState>(LGameState))
	{
		PlayersList->SetListItems(LTPGameState->PlayerArray);
	}

	PlayersList->RegenerateAllEntries();
}

void UThirdPersonLeaderboardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Set the list to the player states
	auto const LGameState = UGameplayStatics::GetGameState(this);
	if(auto const LTPGameState = Cast<AThirdPersonGameState>(LGameState))
	{
		LTPGameState->OnPlayerStateAdded.AddUObject(this, &UThirdPersonLeaderboardWidget::OnPlayerStatesChange);
		LTPGameState->OnPlayerStateRemoved.AddUObject(this, &UThirdPersonLeaderboardWidget::OnPlayerStatesChange);
		PlayersList->SetListItems(LTPGameState->PlayerArray);
	}
	
	PlayersList->RegenerateAllEntries();
}
