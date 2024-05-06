// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThirdPersonMainWindow.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONMP_API UThirdPersonMainWindow : public UUserWidget
{
	GENERATED_BODY()

protected:

	// Override to initialized players num
	virtual void NativeOnInitialized() override;
	
#pragma region Health
public:

	// Updates health variable for this widget
	UFUNCTION(BlueprintCallable, Category="UI|Health")
	void UpdateHealth(float const InNewHealth);
	
protected:

	// Currently replicated player health
	UPROPERTY(BlueprintReadOnly, Category="UI|Health")
	float CurrentPlayerHealth = 0.f;
	
#pragma endregion

#pragma region Stats
	
protected:
	// The amount of players currently in the
	// world that is displayer locally for each user
	UPROPERTY(BlueprintReadOnly, Category="UI|Stats")
	int32 PlayersNum = 0;
	
#pragma endregion
};
