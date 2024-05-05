// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonPlayerController.h"

#include "ThirdPersonMPCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AThirdPersonPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// Bind only at the server controller
	if(GetLocalRole() == ROLE_Authority)
	{
		if(auto const AsThirdPerson = Cast<AThirdPersonMPCharacter>(InPawn))
		{
			AsThirdPerson->GetOnPlayerDied().AddDynamic(this, &AThirdPersonPlayerController::OnControlledPawnDie);
		}
	}
}

void AThirdPersonPlayerController::RespawnPlayer_Implementation()
{
	IThirdPersonControllerInterface::RespawnPlayer_Implementation();

	UGameplayStatics::GetGameMode(this)->RestartPlayer(this);
}

void AThirdPersonPlayerController::OnControlledPawnDie()
{
	Execute_RespawnPlayer(this);
}
