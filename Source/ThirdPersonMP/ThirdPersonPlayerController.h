// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ThirdPersonControllerInterface.h"
#include "ThirdPersonPlayerController.generated.h"

/**
 * Implements ThirdPersonControllerInterface and realises
 * respawning logic
 */
UCLASS()
class THIRDPERSONMP_API AThirdPersonPlayerController : public APlayerController, public IThirdPersonControllerInterface
{
	GENERATED_BODY()

public:

	AThirdPersonPlayerController();
	
#pragma region Gameplay

#pragma region Respawn
	
public:
	//@note: All the functions should be ensured to call on the server side.
	//The binding is ensured to be only on the server
	virtual void OnPossess(APawn* InPawn) override;
	
	// Redirects respawn request to the game mode when character was dead
	virtual void RespawnPlayer_Implementation() override;
	
protected:
	
	// Triggered whenever controlled pawn is dead
	// and is called from the server
	UFUNCTION()
	void OnControlledPawnDie();
	
#pragma endregion

#pragma region Health
	
protected:
	// Triggered when the player's health updated
	UFUNCTION()
	void OnPawnHealthUpdate(float const InUpdatedHealth);
	
#pragma endregion

	// Override to add the UI on the screen
	virtual void BeginPlay() override;

#pragma endregion

#pragma region UI
protected:

	// Class from which we should instantiate our MainWindow widget
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UThirdPersonMainWindow> MainWindowClass;
	
	// The main window of the character being controlled
	UPROPERTY(BlueprintReadOnly, Category="UI")
	class UThirdPersonMainWindow* MainWindow;
	
#pragma endregion
};
