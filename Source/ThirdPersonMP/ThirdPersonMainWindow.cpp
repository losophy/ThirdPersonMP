// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMainWindow.h"

#include "Kismet/GameplayStatics.h"

void UThirdPersonMainWindow::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayersNum = UGameplayStatics::GetNumPlayerStates(this);
}

void UThirdPersonMainWindow::UpdateHealth(float const InNewHealth)
{
	CurrentPlayerHealth = InNewHealth;
}
