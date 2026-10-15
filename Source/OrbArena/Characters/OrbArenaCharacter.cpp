// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaCharacter.h"

#include "EnhancedInputComponent.h"
#include "OrbArenaEnemy.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OrbArena/Objects/OrbArenaEnergyOrb.h"
#include "OrbArena/Objects/OrbArenaProjectile.h"
#include "OrbArena/UI/OrbArenaHealthBarWidget.h"

// Sets default values
AOrbArenaCharacter::AOrbArenaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBar->SetDrawSize(FVector2D(80.f, 10.f));
	HealthBar->SetRelativeLocation(FVector(32.f, 0.f, 50.f));
	
	// // Session 7 - Arrays -- Use as an example
	// ExampleScores.Add(100);
	// ExampleScores.Add(450);
	// ExampleScores.Add(75);
	// ExampleScores.Add(900);
	// ExampleScores.Add(250);
	//
	// ExampleOrbNames.Add("Red Orb");
	// ExampleOrbNames.Add("Green Orb");
	// ExampleOrbNames.Add("Blue Orb");
	// ExampleOrbNames.Add("Gold Orb");
	// ExampleOrbNames.Add("Purple Orb");
	//
	// // Session 8 - Three objects from the FLootDropStruct  -- Use as an example
	// FLootDrop FirstDrop;
	// FirstDrop.ItemName = "Small Orb";
	// FirstDrop.Value = 10;
	// FirstDrop.Rarity = ELootRarity::Common;
	// LootDrops.Add(FirstDrop);
	//
	// FLootDrop SecondDrop;
	// SecondDrop.ItemName = "Silver Orb";
	// SecondDrop.Value = 50;
	// SecondDrop.Rarity = ELootRarity::Rare;
	// LootDrops.Add(SecondDrop);
	//
	// FLootDrop ThirdDrop;
	// ThirdDrop.ItemName = "Golden Orb";
	// ThirdDrop.Value = 100;
	// ThirdDrop.Rarity = ELootRarity::Epic;
	// LootDrops.Add(ThirdDrop);
}

// Called when the game starts or when spawned
void AOrbArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateMovementSpeed();
	FindEnergyOrbs();
	PrintLootDrops();
	UpdateHealthBar();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		ArenaPlayerController = PlayerController;
		
		ArenaPlayerController->SetShowMouseCursor(true);
		
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

// Called every frame
void AOrbArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameState == EGameState::Playing)
	{
		RemainingFireCooldown = FMath::Max(RemainingFireCooldown - DeltaTime, 0.f);
		CheckGameState();
	}
	else
	{
		GetCharacterMovement()->StopMovementImmediately();
	}

	PrintTextToScreen();
	PrintPlayerLocation();
}

// Called to bind functionality to input
void AOrbArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOrbArenaCharacter::MovePlayer);

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AOrbArenaCharacter::StartSprint);

	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AOrbArenaCharacter::StopSprint);

	EnhancedInputComponent->BindAction(DamageAction, ETriggerEvent::Triggered, this, &AOrbArenaCharacter::DamageInput);

	EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Triggered, this, &AOrbArenaCharacter::HealInput);

	EnhancedInputComponent->BindAction(AddScoreAction, ETriggerEvent::Triggered, this, &AOrbArenaCharacter::AddScoreInput);

	EnhancedInputComponent->BindAction(DamageEnemyAction, ETriggerEvent::Triggered, this, &AOrbArenaCharacter::ShootProjectile);
}

void AOrbArenaCharacter::MovePlayer(const FInputActionValue& Value)
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	FVector2D MovementValue = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementValue.X);
	AddMovementInput(GetActorRightVector(), MovementValue.Y);
}

void AOrbArenaCharacter::StartSprint()
{
	bIsSprinting = true;
	UpdateMovementSpeed();
}

void AOrbArenaCharacter::StopSprint()
{
	bIsSprinting = false;
	UpdateMovementSpeed();
}

void AOrbArenaCharacter::DamageInput()
{
	ApplyDamageAmount(TempDamage);
}

void AOrbArenaCharacter::HealInput()
{
	HealPlayer(TempHeal);
}

void AOrbArenaCharacter::AddScoreInput()
{
	AddScore(TempPoints);
}

void AOrbArenaCharacter::DamageNearestEnemyInput()
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrbArenaEnemy::StaticClass(), FoundActors);

	AOrbArenaEnemy* NearestEnemy = nullptr;
	float NearestDistance = 0.f;

	for (AActor* Actor : FoundActors)
	{
		AOrbArenaEnemy* Enemy = Cast<AOrbArenaEnemy>(Actor);

		if (!IsValid(Enemy))
		{
			continue;
		}

		float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());

		if (NearestEnemy == nullptr || Distance < NearestDistance)
		{
			NearestEnemy = Enemy;
			NearestDistance = Distance;
		}
	}

	if (IsValid(NearestEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("Dealing %f damage to %s"), PlayerStats.Damage, *NearestEnemy->GetName());
		NearestEnemy->ApplyDamageAmount(PlayerStats.Damage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No enemy found."));
	}
}

void AOrbArenaCharacter::ShootProjectile()
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	if (RemainingFireCooldown > 0.f)
	{
		return;
	}

	FHitResult CursorHit;
	ArenaPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	FVector ShootDirection = CursorHit.ImpactPoint - GetActorLocation();

	ShootDirection.Z = 0.f;
	ShootDirection = ShootDirection.GetSafeNormal();

	FVector SpawnLocation = GetActorLocation() + ShootDirection * 45.f;

	FRotator SpawnRotation = ShootDirection.Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AOrbArenaProjectile* Projectile = GetWorld()->SpawnActor<AOrbArenaProjectile>(
		ProjectileBlueprint, 
		SpawnLocation, 
		SpawnRotation, 
		SpawnParameters
		);

	if (IsValid(Projectile))
	{
		RemainingFireCooldown = FireCooldown;
		UE_LOG(LogTemp, Warning, TEXT("Projectile fired. Next shot in %.2f seconds"), FireCooldown);
	}
}

void AOrbArenaCharacter::UpdateMovementSpeed()
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	float DifficultyMultiplier = MovementMultiplierEasy;

	if (GameDifficulty == EGameDifficulty::Medium)
	{
		DifficultyMultiplier = MovementMultiplierMedium;
	}
	else if (GameDifficulty == EGameDifficulty::Hard)
	{
		DifficultyMultiplier = MovementMultiplierHard;
	}

	float ActiveSprintMultiplier = 1.f;

	if (bIsSprinting)
	{
		ActiveSprintMultiplier = SprintMultiplier;
	}

	CurrentMovementSpeed = PlayerStats.MovementSpeed * DifficultyMultiplier * ActiveSprintMultiplier;
	CurrentMovementSpeed = FMath::Max(CurrentMovementSpeed, 0.f);

	GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSpeed;
}

void AOrbArenaCharacter::AddScore(int32 InAmount)
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	Score += InAmount;

	if (Score > 1000)
	{
		GameDifficulty = EGameDifficulty::Hard;
	}
	else if (Score >= 500)
	{
		GameDifficulty = EGameDifficulty::Medium;
	}
	else
	{
		GameDifficulty = EGameDifficulty::Easy;
	}

	UpdateMovementSpeed();

	UE_LOG(LogTemp, Warning, TEXT("New score is %d"), Score);
	CheckGameState();
}

void AOrbArenaCharacter::HealPlayer(float InAmount)
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	PlayerStats.Health += FMath::Max(InAmount, 0.f);
	PlayerStats.Health = FMath::Clamp(PlayerStats.Health, 0, PlayerStats.MaxHealth);
	UpdateHealthBar();

	UE_LOG(LogTemp, Warning, TEXT("Player healed, new Health is %.2f"), PlayerStats.Health);
}

void AOrbArenaCharacter::ApplyDamageAmount(float InAmount)
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	PlayerStats.Health -= FMath::Max(InAmount, 0.f);
	PlayerStats.Health = FMath::Clamp(PlayerStats.Health, 0.f, PlayerStats.MaxHealth);
	
	UpdateHealthBar();

	UE_LOG(LogTemp, Warning, TEXT("Player damaged, new Health is %.2f"), PlayerStats.Health);

	if (PlayerStats.MaxHealth > 0.f && PlayerStats.Health <= PlayerStats.MaxHealth * 0.25f)
	{
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(30, 2.f, FColor::Red, TEXT("LOW HEALTH!!!"));
		}
	}

	if (PlayerStats.Health <= 0.f && PlayerLives > 0)
	{
		LoseLife();
	}

	CheckGameState();
}

void AOrbArenaCharacter::LoseLife()
{
	PlayerLives -= 1;
	PlayerLives = FMath::Max(PlayerLives, 0);

	UE_LOG(LogTemp, Warning, TEXT("Player died! Lives remaining: %d"), PlayerLives);

	if (PlayerLives > 0)
	{
		PlayerStats.Health = PlayerStats.MaxHealth;
		UpdateHealthBar();
	}
	else
	{
		GameState = EGameState::Lost;
		GetCharacterMovement()->StopMovementImmediately();
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER!!"));
	}
}

void AOrbArenaCharacter::UpdateHealthBar()
{
	UOrbArenaHealthBarWidget* HealthBarWidget = Cast<UOrbArenaHealthBarWidget>(HealthBar->GetUserWidgetObject());
	
	if (IsValid(HealthBarWidget))
	{
		HealthBarWidget->SetHealth(PlayerStats.Health, PlayerStats.MaxHealth);
	}
}

// Session 7 - Finding every orb
void AOrbArenaCharacter::FindEnergyOrbs()
{
	EnergyOrbArray.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrbArenaEnergyOrb::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AOrbArenaEnergyOrb* Orb = Cast<AOrbArenaEnergyOrb>(Actor);

		if (IsValid(Orb))
		{
			EnergyOrbArray.Add(Orb);
		}
	}

	StartingOrbCount = EnergyOrbArray.Num();
	bAllOrbsCollected = false;
	UpdateRemainingOrbCount();
}

void AOrbArenaCharacter::UpdateRemainingOrbCount()
{
	// Subtracting one orb from the Energy Orb Array
	for (int32 Index = EnergyOrbArray.Num() - 1; Index >= 0; Index--)
	{
		// We use IsValid here because orb can be destroyed while it is stil referencing it
		// First we check if the orb still exists. Only if it does exist we check if it has been picked up
		if (!IsValid(EnergyOrbArray[Index]) || EnergyOrbArray[Index]->bHasBeenPickedUp)
		{
			EnergyOrbArray.RemoveAt(Index);
		}
	}

	OrbsRemaining = EnergyOrbArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("Orbs remaining: %d"), OrbsRemaining);

	// Checking if we collected all the orbs, and that it does instantly win if the map has no orbs in it from start
	if (StartingOrbCount > 0 && OrbsRemaining == 0 && !bAllOrbsCollected)
	{
		bAllOrbsCollected = true;
		UE_LOG(LogTemp, Warning, TEXT("All Orbs collected!"));
		CheckGameState();
	}
}

void AOrbArenaCharacter::PrintLootDrops()
{
	for (AOrbArenaEnergyOrb* Orb : EnergyOrbArray)
	{
		// FString RarityName = TEXT("Common");
		//
		// if (Orb->OrbLootDrop.Rarity == ELootRarity::Rare)
		// {
		// 	RarityName = TEXT("Rare");
		// }
		// else if (Orb->OrbLootDrop.Rarity == ELootRarity::Epic)
		// {
		// 	RarityName = TEXT("Epic");
		// }

		FString RarityName = StaticEnum<ELootRarity>()->GetNameStringByValue(static_cast<int64>(Orb->OrbLootDrop.Rarity));

		UE_LOG(LogTemp, Warning, TEXT("Loot: %s | Rarity: %s | Value: %d"), *Orb->OrbLootDrop.ItemName, *RarityName, Orb->OrbLootDrop.Value);
	}
}


void AOrbArenaCharacter::CheckGameState()
{
	if (GameState != EGameState::Playing)
	{
		return;
	}

	if (PlayerLives <= 0)
	{
		GameState = EGameState::Lost;
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER!!"));
	}
	else if (Score >= ScoreWinCondition)
	{
		GameState = EGameState::Won;
		UE_LOG(LogTemp, Warning, TEXT("YOU WON!!!"));
	}
	else if (bWinWhenAllOrbsCollected && bAllOrbsCollected)
	{
		GameState = EGameState::Won;
		UE_LOG(LogTemp, Warning, TEXT("YOU WON: all Orbs collected!"));
	}
}

void AOrbArenaCharacter::PrintPlayerLocation()
{
	FVector PlayerLocation = GetActorLocation();

	GEngine->AddOnScreenDebugMessage(11, 0.1f, FColor::Yellow, FString::Printf(TEXT("Player Location - X: %.1f, Y: %.1f, Z: %.1f"), PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z));
}

void AOrbArenaCharacter::PrintTextToScreen()
{
	int DebugKey = 1;

	if (GameState == EGameState::Playing)
	{
		GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Purple, FString::Printf(TEXT("Game State: Playing")));
	}
	else if (GameState == EGameState::Won)
	{
		GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Purple, FString::Printf(TEXT("Game State: Won")));
	}
	else if (GameState == EGameState::Lost)
	{
		GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Purple, FString::Printf(TEXT("Game State: Lost")));
	}
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Cyan, FString::Printf(TEXT("----------------------")));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Emerald, FString::Printf(TEXT("Player Name is: %s"), *PlayerName));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Emerald, FString::Printf(TEXT("Player Level is: %d"), PlayerLevel));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Emerald, FString::Printf(TEXT("Score is: %d"), Score));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Emerald, FString::Printf(TEXT("Current Lives is: %d"), PlayerLives));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Cyan, FString::Printf(TEXT("----------------------")));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Red, FString::Printf(TEXT("Health is: %.2f"), PlayerStats.Health));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Red, FString::Printf(TEXT("Max Health is: %.2f"), PlayerStats.MaxHealth));
	DebugKey++;

	if (CurrentMovementSpeed >= 2000.f)
	{
		GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Red, FString::Printf(TEXT("PLAYER IS SONIC!! Speed is: %.2f"), CurrentMovementSpeed));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Red, FString::Printf(TEXT("Movement Speed is: %.2f"), CurrentMovementSpeed));
	}
}

// EXERCISES - USE AS EXAMPLES ----------------------------------- //
// void AOrbArenaCharacter::RunArrayExercises()
// {
// 	// For-loop printing highest score
// 	if (ExampleScores.Num() > 0)
// 	{
// 		int HighestScore = 0;
//
// 		for (int32 Index = 0; Index < ExampleScores.Num(); Index++)
// 		{
// 			if (ExampleScores[Index] > HighestScore)
// 			{
// 				HighestScore = ExampleScores[Index];
// 			}
// 		}
//
// 		UE_LOG(LogTemp, Warning, TEXT("Highest score: %d"), HighestScore);
// 	}
//
// 	// Range-based loop printing every name.
// 	for (FString Name : ExampleOrbNames)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Orb name: %s"), *Name);
// 	}
//
// 	// Range-based loop searching for the input of "OrbNameToFind".
// 	bool bFoundName = false;
//
// 	for (FString Name : ExampleOrbNames)
// 	{
// 		if (Name == OrbNameToFind)
// 		{
// 			bFoundName = true;
// 			break;
// 		}
// 	}
// 	if (bFoundName)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Found Orb name: %s"), *OrbNameToFind);
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Orb name not found: %s"), *OrbNameToFind);
// 	}
// }
//
// void AOrbArenaCharacter::PrintStructExercise()
// {
// 	UE_LOG(LogTemp, Warning,
// 		   TEXT("Player Stats: Health %.1f / %.1f, Speed %.1f, Damage %.1f"),
// 		   PlayerStats.Health,
// 		   PlayerStats.MaxHealth,
// 		   PlayerStats.MovementSpeed,
// 		   PlayerStats.Damage);
// }
//
// FLootDrop AOrbArenaCharacter::GetHighestValueLootDrop()
// {
// 	if (LootDrops.Num() == 0)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("No loot"));
// 		return FLootDrop();
// 	}
//
// 	FLootDrop HighestDrop;
//
// 	for (int32 Index = 0; Index < LootDrops.Num(); Index++)
// 	{
// 		if (LootDrops[Index].Value > HighestDrop.Value)
// 		{
// 			HighestDrop = LootDrops[Index];
// 		}
// 	}
//
// 	return HighestDrop;
// }

// END EXERCISE -------------------------------------------
