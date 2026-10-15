// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaEnemy.h"

#include "OrbArenaCharacter.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OrbArena/UI/OrbArenaHealthBarWidget.h"


AOrbArenaEnemy::AOrbArenaEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	EnemyHealthBarComponent = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	EnemyHealthBarComponent->SetupAttachment(GetRootComponent());
	EnemyHealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

void AOrbArenaEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateHealth();

	PlayerCharacter = Cast<AOrbArenaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AOrbArenaEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(PlayerCharacter) || PlayerCharacter->GameState != EGameState::Playing)
	{
		return;
	}

	AttackCooldownRemaining = FMath::Max(AttackCooldownRemaining - DeltaTime, 0.f);

	UpdateVisionAndTurning(DeltaTime);

	if (bMoveTowardsPlayer)
	{
		MoveTowardsPlayer(DeltaTime);
	}

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

	if (Distance <= EnemyStats.AttackRange && bCanSeePlayer)
	{
		PlayerCharacter->ApplyDamageAmount(EnemyStats.Damage);

		AttackCooldownRemaining = EnemyStats.AttackCooldown;

		UE_LOG(LogTemp, Warning, TEXT("%s attacked %s for %.1f damage."), *EnemyName, *PlayerCharacter->PlayerName, EnemyStats.Damage);
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
	UpdateHealth();

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

	if (bCanSeePlayer)
	{
		DebugColor = FColor::Green;
	}

	FVector Location = GetActorLocation() - FVector(0.f, 0.f, 45.f);

	DrawDebugSphere(GetWorld(), Location, EnemyStats.AttackRange, 16, DebugColor, false, 0.f, 0, 2.f);

	DrawDebugDirectionalArrow(GetWorld(), Location, Location + GetActorForwardVector() * 150.f, 30.f, DebugColor, false, 0.f, 0, 3.f);
}

void AOrbArenaEnemy::UpdateHealth()
{
	UOrbArenaHealthBarWidget* HealthBarWidget = Cast<UOrbArenaHealthBarWidget>(EnemyHealthBarComponent->GetUserWidgetObject());
	
	if (IsValid(HealthBarWidget))
	{
		HealthBarWidget->SetHealth(EnemyStats.Health, EnemyStats.MaxHealth);
	}
}

void AOrbArenaEnemy::UpdateVisionAndTurning(float DeltaTime)
{
	FVector ToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.f;
	FVector ToPlayerDirection = ToPlayer.GetSafeNormal();

	FVector EnemyForward = GetActorForwardVector();
	EnemyForward.Z = 0.f;
	EnemyForward = EnemyForward.GetSafeNormal();

	// Dot Product
	DotProductValue = FVector::DotProduct(EnemyForward, ToPlayerDirection);
	bCanSeePlayer = DotProductValue > DotProductValueToSeePlayer;

	if (bLogDotCrossProduct)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s | Dot: %.3f | Cross: %.3f"), *EnemyName, DotProductValue, FVector::CrossProduct(EnemyForward, ToPlayerDirection).Z);
	}

	if (bRequireVisionToTurn && !bCanSeePlayer)
	{
		return;
	}

	if (DotProductValue > DotProductValueToStopTurning)
	{
		return;
	}

	// Cross Product
	CrossProductValue = FVector::CrossProduct(EnemyForward, ToPlayerDirection);

	float TurnDirection = 1.f;

	if (CrossProductValue.Z < 0.f)
	{
		TurnDirection = -1.f;
	}

	float TurnAmount = TurnDirection * TurnSpeed * DeltaTime;
	AddActorLocalRotation(FRotator(0.f, TurnAmount, 0.f));
}

void AOrbArenaEnemy::MoveTowardsPlayer(float DeltaTime)
{
	if (!bCanSeePlayer)
	{
		return;
	}

	FVector ToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.f;

	float Distance = ToPlayer.Size();

	if (Distance <= EnemyStats.AttackRange)
	{
		return;
	}

	FVector Direction = ToPlayer.GetSafeNormal();
	float MoveDistance = FMath::Max(EnemyStats.MovementSpeed, 0.f) * DeltaTime;

	MoveDistance = FMath::Min(MoveDistance, Distance - EnemyStats.AttackRange);

	SetActorLocation(GetActorLocation() + Direction * MoveDistance);
}
