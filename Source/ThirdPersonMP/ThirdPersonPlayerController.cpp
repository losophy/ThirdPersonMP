// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonPlayerController.h"

#include "EnhancedInputComponent.h"
#include "ThirdPersonMPCharacter.h"
#include "ThirdPersonMainWindow.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AThirdPersonPlayerController::AThirdPersonPlayerController()
{
	
	// Set the default class to the blueprinted one
	static ConstructorHelpers::FClassFinder<UThirdPersonMainWindow>
	DefaultMainWindowClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonMainWindow.BP_ThirdPersonMainWindow"));

	if(DefaultMainWindowClass.Class)
	{
		MainWindowClass = DefaultMainWindowClass.Class;
	}
	
	// Enable replication for this controller
	bReplicates = true;
}

void AThirdPersonPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// Bind only at the server controller
	if(GetLocalRole() == ROLE_Authority)
	{
		if(auto const AsThirdPerson = Cast<AThirdPersonMPCharacter>(InPawn))
		{
			AsThirdPerson->GetOnPlayerDied().AddUniqueDynamic(this, &AThirdPersonPlayerController::OnControlledPawnDie);
			AsThirdPerson->GetOnPlayerHealthUpdate().AddUniqueDynamic(this, &AThirdPersonPlayerController::OnPawnHealthUpdate);
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

void AThirdPersonPlayerController::OnPawnHealthUpdate(float const InUpdatedHealth)
{
	// Client-only logic
	if(IsLocalController())
	{
		MainWindow->UpdateHealth(InUpdatedHealth);
	}
}

void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// We want to create UI only locally for a player.
	if(IsLocalController())
	{
		// Create main window
		MainWindow = CreateWidget<UThirdPersonMainWindow>(this, MainWindowClass);
		MainWindow->AddToViewport();
	}
	
}

void AThirdPersonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		ensureMsgf(TabAction, TEXT("Bad action reference!"));

		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &AThirdPersonPlayerController::OnTabActionStarted);
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Completed, this,
		                           &AThirdPersonPlayerController::OnTabActionCompleted);
	}
}

void AThirdPersonPlayerController::OnTabActionStarted()
{
	if(GetLocalRole() != ROLE_Authority)
	{
		MainWindow->DisplayLeaderboard();
	}
}

void AThirdPersonPlayerController::OnTabActionCompleted()
{
	if(GetLocalRole() != ROLE_Authority)
	{
		MainWindow->HideLeaderboard();
	}
}
