// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaHUDWidget.h"

#include "Components/TextBlock.h"
#include "OrbArena/Characters/OrbArenaCharacter.h"

void UOrbArenaHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	AOrbArenaCharacter* Player = Cast<AOrbArenaCharacter>(GetOwningPlayerPawn());
	
	if (!IsValid(Player))
	{
		return;
	}
	
	HealthText->SetText(FText::FromString(FString::Printf(TEXT("Health: %.0f"), Player->PlayerStats.Health)));
	ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), Player->Score)));
}
