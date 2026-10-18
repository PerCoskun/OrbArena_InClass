// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaEndScreenWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "OrbArena/Characters/OrbArenaCharacter.h"

void UOrbArenaEndScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(RestartButton))
	{
		RestartButton->OnClicked.AddDynamic(this, &UOrbArenaEndScreenWidget::RestartClicked);
	}
}

void UOrbArenaEndScreenWidget::ShowResult(AOrbArenaCharacter* InPlayer)
{
	Player = InPlayer;

	if (!IsValid(Player))
	{
		return;
	}

	if (Player->GameState == EGameState::Won)
	{
		ResultText->SetText(FText::FromString(TEXT("YOU WON!")));

		if (Player->Score >= GreatScoreThreshold)
		{
			ResultText->SetText(FText::FromString(TEXT("YOU WON! Great score!")));
		}
		else
		{
			ResultText->SetText(FText::FromString(TEXT("YOU WON! Just made it!")));
		}
	}
	else
	{
		ResultText->SetText(FText::FromString(TEXT("GAME OVER")));
	}
}

void UOrbArenaEndScreenWidget::RestartClicked()
{
	if (IsValid(Player))
	{
		Player->RestartGame();
	}
}
