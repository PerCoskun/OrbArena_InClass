// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaEnemy.h"

#include "OrbArenaCharacter.h"
#include "Kismet/GameplayStatics.h"


AOrbArenaEnemy::AOrbArenaEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOrbArenaEnemy::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AOrbArenaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AOrbArenaEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(PlayerCharacter) || PlayerCharacter->GameState != EGameState::Playing)
	{
		return;
	}

	// Simple count down using delta time (seconds)
	AttackCooldownRemaining = FMath::Max(AttackCooldownRemaining - DeltaTime, 0.f);

	AttackPlayer();
	Debugging();
}

void AOrbArenaEnemy::AttackPlayer()
{
	if (!IsValid(PlayerCharacter) || PlayerCharacter->GameState != EGameState::Playing)
	{
		return;
	}

	if (AttackCooldownRemaining > 0.f)
	{
		return;
	}

	float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

	if (Distance <= EnemyStats.AttackRange)
	{
		PlayerCharacter->ApplyDamageAmount(EnemyStats.Damage);
		
		// Reset timer after dealing damage (each enemy has its own cooldown).
		AttackCooldownRemaining = EnemyStats.AttackCooldown;

		UE_LOG(LogTemp, Warning, TEXT("%s attacked %s for %.1f damage."), *EnemyName, *PlayerCharacter->PlayerName ,EnemyStats.Damage);
	}
}

void AOrbArenaEnemy::ApplyDamageAmount(float InAmount)
{
	if (EnemyStats.Health <= 0.f)
	{
		return;
	}

	EnemyStats.Health -= FMath::Max(InAmount, 0.f);
	EnemyStats.Health = FMath::Clamp(EnemyStats.Health, 0.f, EnemyStats.MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Ouch, %s is damaged. Health remaining: %.2f"), *EnemyName, EnemyStats.Health);

	if (EnemyStats.Health <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s died and is being destroyed."), *EnemyName);
		Destroy();
	}
}

void AOrbArenaEnemy::Debugging()
{
	if (!bDrawDebug)
	{
		return;
	}

	FColor DebugColor = FColor::Red;

	FVector Location = GetActorLocation() - FVector(0.f, 0.f, 45.f);

	DrawDebugSphere(GetWorld(), Location, EnemyStats.AttackRange, 16, DebugColor, false, 0.f, 0, 2.f);

	DrawDebugDirectionalArrow(GetWorld(), Location, Location + GetActorForwardVector() * 150.f, 30.f, DebugColor, false, 0.f, 0, 3.f);
}
