// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaEnergyOrb.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "OrbArena/Characters/OrbArenaCharacter.h"


AOrbArenaEnergyOrb::AOrbArenaEnergyOrb()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOrbArenaEnergyOrb::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AOrbArenaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AOrbArenaEnergyOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PlayerCharacter->GameState != EGameState::Playing)
	{
		return;
	}

	AddActorLocalRotation(FRotator{OrbRotationSpeed * DeltaTime, OrbRotationSpeed, OrbRotationSpeed});

	if (IsValid(PlayerCharacter))
	{
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		FVector OrbLocation = GetActorLocation();

		DrawDebugSphere(GetWorld(), GetActorLocation(), PickupRadius, 10, OrbDebugColor, false, 0.1f, 0, 2.f);

		if (FVector::Dist(OrbLocation, PlayerLocation) <= PickupRadius)
		{
			if (bHasBeenPickedUp)
			{
				return;
			}

			OrbPickedUp(OrbLootDrop.Value);
		}
	}
}

void AOrbArenaEnergyOrb::OrbPickedUp(int32 InPointWorth)
{
	if (IsValid(PlayerCharacter) && !bHasBeenPickedUp)
	{
		bHasBeenPickedUp = true;
		PlayerCharacter->AddScore(InPointWorth);
		
		if (IsValid(PickupSFX))
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSFX, GetActorLocation());
		}
		
		UE_LOG(LogTemp, Warning, TEXT("PICKED UP!! Gained %d points"), OrbLootDrop.Value);
		PlayerCharacter->UpdateRemainingOrbCount();

		Destroy();
	}
}
