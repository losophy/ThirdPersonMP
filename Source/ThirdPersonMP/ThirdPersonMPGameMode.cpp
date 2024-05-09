// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonMPGameMode.h"

#include "ThirdPersonGameState.h"
#include "ThirdPersonMPCharacter.h"
#include "ThirdPersonPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogTPGameMode, Log, All);

AThirdPersonMPGameMode::AThirdPersonMPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set default controller class to our Blueprinted player controller
	static ConstructorHelpers::FClassFinder<AController> PlayerControllerBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController.BP_ThirdPersonPlayerController"));
	if(PlayerControllerBPClass.Class)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	else
	{
		PlayerControllerClass = AThirdPersonPlayerController::StaticClass();
	}
	
}

void AThirdPersonMPGameMode::RestartPlayer(AController* NewPlayer)
{
	if(NewPlayer)
	{
		NewPlayer->Possess(
			GetWorld()->SpawnActor<APawn>(DefaultPawnClass,
				FindPlayerStart(NewPlayer)->GetTransform()) 
		);
	}
}

void AThirdPersonMPGameMode::HandlePlayerKilled_Implementation(AController* InKiller, AController* InVictim)
{
	IThirdPersonGameModeInterface::HandlePlayerKilled_Implementation(InKiller, InVictim);

	if((InKiller && InVictim) && InKiller != InVictim)
	{
		IThirdPersonStatsInterface::Execute_AddKill(InKiller->PlayerState);
		IThirdPersonStatsInterface::Execute_AddDeath(InVictim->PlayerState);

		UE_LOG(LogTPGameMode, Log, TEXT("Player %s killed %s"), *GetNameSafe(InKiller),
			*GetNameSafe(InVictim));
	}

}

void AThirdPersonMPGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(auto const LTPGameState = Cast<AThirdPersonGameState>(GameState);
		ensureMsgf(LTPGameState, TEXT("Bad game state cast! Game state: %s"), *GetNameSafe(GameState)))
	{
		LTPGameState->SetConnectedPlayers(LTPGameState->ConnectedPlayers + 1);
	}
}

void AThirdPersonMPGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if(auto const LTPGameState = Cast<AThirdPersonGameState>(GameState);
		ensureMsgf(LTPGameState, TEXT("Bad game state cast! Game state: %s"), *GetNameSafe(GameState)))
	{
		LTPGameState->SetConnectedPlayers(LTPGameState->ConnectedPlayers - 1);
	}
}
