// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OrbArenaEndScreenWidget.generated.h"

class UTextBlock;
class UButton;
class AOrbArenaCharacter;

UCLASS()
class ORBARENA_API UOrbArenaEndScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowResult(AOrbArenaCharacter* InPlayer);

	UPROPERTY(EditAnywhere, Category = "End Screen")
	int32 GreatScoreThreshold = 1000;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultText;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* RestartButton;

private:
	UFUNCTION()
	void RestartClicked();

	UPROPERTY()
	AOrbArenaCharacter* Player = nullptr;
};
