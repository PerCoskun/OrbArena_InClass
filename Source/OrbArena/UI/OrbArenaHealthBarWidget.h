// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OrbArenaHealthBarWidget.generated.h"

class UProgressBar;

UCLASS()
class ORBARENA_API UOrbArenaHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float InHealth, float InMaxHealth);

protected:
	// Progress Bar with this exact name in the Widget Blueprint
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
};
