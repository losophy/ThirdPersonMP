// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonLeaderboardWidget.h"

#include "ThirdPersonStatsComponent.h"

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
