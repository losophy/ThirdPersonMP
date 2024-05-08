// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonGameState.h"

AThirdPersonGameState::AThirdPersonGameState()
{
	// Set the default gamemode class to the bluprinted one
	static ConstructorHelpers::FClassFinder<AGameModeBase> DefaultGameMode(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonMPGameMode.BP_ThirdPersonMPGameMode"));

	if(DefaultGameMode.Succeeded())
	{
		GameModeClass = DefaultGameMode.Class;
	}
	
	
}
