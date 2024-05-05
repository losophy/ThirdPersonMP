// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonMPGameMode.h"

#include "ThirdPersonMPCharacter.h"
#include "ThirdPersonPlayerController.h"
#include "UObject/ConstructorHelpers.h"

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
