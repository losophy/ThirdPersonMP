// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonLeaderboardWidget.h"
#include "ThirdPersonMPCharacter.h"
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
	}
}

void UThirdPersonLeaderboardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Set the list
	auto const GameState = UGameplayStatics::GetGameState(this);
	PlayersList->SetListItems(GameState->PlayerArray);
}
