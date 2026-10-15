// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UOrbArenaHealthBarWidget::SetHealth(float InHealth, float InMaxHealth)
{
	if (IsValid(HealthBar) && InMaxHealth > 0.f)
	{
		HealthBar->SetPercent(InHealth / InMaxHealth);
	}
}
