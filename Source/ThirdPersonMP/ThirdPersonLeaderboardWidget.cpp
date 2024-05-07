// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonLeaderboardWidget.h"
#include "ThirdPersonMPCharacter.h"
#include "ThirdPersonStatsComponent.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"

void UThirdPersonLeaderboardElement::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if(ListItemObject &&
		ensureMsgf(ListItemObject->IsA(UThirdPersonStatsComponent::StaticClass()),
			TEXT("Bad list item cast!")))
	{
		// Initialize player stat component to draw data from it.
		PlayerStatComponent = Cast<UThirdPersonStatsComponent>(ListItemObject);
	}
}

void UThirdPersonLeaderboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//@note: the replication of stats is built-in in the component, so we just need to show
	// them to the local player

	// Gather all playable characters
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, AThirdPersonMPCharacter::StaticClass(),OutActors);

	// Extract stat components from each character
	TArray<UThirdPersonStatsComponent*> StatComponents;
	Algo::Transform(OutActors, StatComponents, [](AActor* InActor)
	{
		return InActor->FindComponentByClass<UThirdPersonStatsComponent>();
	});

	// Trim any null references
	StatComponents = StatComponents.FilterByPredicate([](UThirdPersonStatsComponent* InComp)
		{ return static_cast<bool>(InComp);});

	ensureMsgf(OutActors.Num() == StatComponents.Num(), TEXT("Some stat compnents were not found!"));

	// Set the list
	PlayersList->SetListItems(StatComponents);
}
