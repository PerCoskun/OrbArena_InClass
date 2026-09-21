// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaCharacter.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AOrbArenaCharacter::AOrbArenaCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOrbArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateMovementSpeed();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
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

	PrintTextToScreen();
	PrintPlayerLocation();

	if (GameState == EGameState::Playing)
	{
		if (Health <= 0 && PlayerLives > 0)
		{
			LoseLife();
		}

		CheckGameState();
	}
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
	// Either return early if the game state is not playing, or only do action if the gamestate is playing
	if (GameState != EGameState::Playing)
	{
		return;
	}

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
	// Either return early if the game state is not playing, or only do action if the gamestate is playing
	if (GameState == EGameState::Playing)
	{
		ApplyDamageAmount(TempDamage);
	}
}

void AOrbArenaCharacter::HealInput()
{
	// Either return early if the game state is not playing, or only do action if the gamestate is playing
	if (GameState == EGameState::Playing)
	{
		HealPlayer(TempHeal);
	}
}

void AOrbArenaCharacter::AddScoreInput()
{
	// Either return early if the game state is not playing, or only do action if the gamestate is playing
	if (GameState == EGameState::Playing)
	{
		AddScore(TempPoints);
	}
}

void AOrbArenaCharacter::UpdateMovementSpeed()
{
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

	CurrentMovementSpeed = BaseMovementSpeed * DifficultyMultiplier * ActiveSprintMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSpeed;
}

void AOrbArenaCharacter::AddScore(int32 InAmount)
{
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
}

void AOrbArenaCharacter::HealPlayer(float InAmount)
{
	Health += InAmount;
	Health = FMath::Clamp(Health, 0, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Player healed, new Health is %.2f"), Health);
}

void AOrbArenaCharacter::ApplyDamageAmount(float InAmount)
{
	Health -= InAmount;
	Health = FMath::Clamp(Health, 0, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Player damaged, new Health is %.2f"), Health);

	if (MaxHealth > 0.f && Health / MaxHealth <= 0.25f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Emerald, FString::Printf(TEXT("LOW HEALTH!!!")));
	}
}

void AOrbArenaCharacter::LoseLife()
{
	PlayerLives -= 1;
	PlayerLives = FMath::Max(PlayerLives, 0);

	if (PlayerLives == 0)
	{
		GameState = EGameState::Lost;
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER!!"));
		return;
	}

	Health = MaxHealth;
	UE_LOG(LogTemp, Warning, TEXT("Player died! Current lives: %d"), PlayerLives);
}

void AOrbArenaCharacter::CheckGameState()
{
	if (PlayerLives == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER!!"));
		GameState = EGameState::Lost;
	}
	else if (Score >= ScoreWinCondition)
	{
		UE_LOG(LogTemp, Warning, TEXT("YOU WON!!!"));
		GameState = EGameState::Won;
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

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Red, FString::Printf(TEXT("Health is: %.2f"), Health));
	DebugKey++;

	GEngine->AddOnScreenDebugMessage(DebugKey, 0.1f, FColor::Red, FString::Printf(TEXT("Max Health is: %.2f"), MaxHealth));
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

// void AOrbArenaCharacter::CheckKeyPressed()
// {
// 	if (APlayerController* PC = Cast<APlayerController>(GetController()))
// 	{
// 		if (PC->IsInputKeyDown(EKeys::One))
// 		{
// 			ApplyDamageAmount(TempDamage);
// 		}
//
// 		if (PC->IsInputKeyDown(EKeys::Two))
// 		{
// 			HealPlayer(TempHeal);
// 		}
//
// 		if (PC->IsInputKeyDown(EKeys::Three))
// 		{
// 			AddScore(TempPoints);
// 		}
// 	}
// }
//
// //Mathematical usage of vector concept
// void AOrbArenaCharacter::MovePlayer(float DeltaTime)
// {
// 	//TODO: take the input direction, make it length 1, multiply it by how fast the player should move and how much time passed, then add that small movement to the player’s current position.
//
// 	FVector MoveDirection = FVector::ZeroVector;
//
// 	// Current player location - p = (x,y,z)
// 	FVector CurrentLocation = GetActorLocation();
//
// 	if (APlayerController* PC = Cast<APlayerController>(GetController()))
// 	{
// 		// X axis = forward/backward in world space
// 		if (PC->IsInputKeyDown(EKeys::W))
// 		{
// 			MoveDirection.X += 1.f;
// 		}
//
// 		if (PC->IsInputKeyDown(EKeys::S))
// 		{
// 			MoveDirection.X -= 1.f;
// 		}
//
// 		// Y axis = left/right in world space
// 		if (PC->IsInputKeyDown(EKeys::D))
// 		{
// 			MoveDirection.Y += 1.f;
// 		}
//
// 		if (PC->IsInputKeyDown(EKeys::A))
// 		{
// 			MoveDirection.Y -= 1.f;
// 		}
// 	}
//
// 	// Pressing W gives d = (1,0,0)
// 	// Pressing W and D together gives d = (1,1,0)
// 	// Normalize to prevent diagonal movement from being 41% faster.
// 	MoveDirection = MoveDirection.GetSafeNormal();
//
// 	// Distance = direction × speed × time
// 	const FVector MovementThisFrame = MoveDirection * CurrentMovementSpeed * DeltaTime;
//
// 	// New position = old position + movement
// 	const FVector NewLocation = CurrentLocation + MovementThisFrame;
//
// 	SetActorLocation(NewLocation);
// }
//
// void AOrbArenaCharacter::PlayerSprint()
// {
// 	if (APlayerController* PC = Cast<APlayerController>(GetController()))
// 	{
// 		bIsSprinting = PC->IsInputKeyDown(EKeys::LeftShift);
//
// 		float DifficultyMultiplier = MovementMultiplierEasy;
//
// 		if (GameDifficulty == EGameDifficulty::Medium)
// 		{
// 			DifficultyMultiplier = MovementMultiplierMedium;
// 		}
// 		else if (GameDifficulty == EGameDifficulty::Hard)
// 		{
// 			DifficultyMultiplier = MovementMultiplierHard;
// 		}
//
// 		float ActiveSprintMultiplier;
// 		if (bIsSprinting)
// 		{
// 			ActiveSprintMultiplier = SprintMultiplier;
// 		}
// 		else
// 		{
// 			ActiveSprintMultiplier = 1.f;
// 		}
//
// 		CurrentMovementSpeed = BaseMovementSpeed * DifficultyMultiplier * ActiveSprintMultiplier;
//
// 		GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSpeed;
// 	}
// }
