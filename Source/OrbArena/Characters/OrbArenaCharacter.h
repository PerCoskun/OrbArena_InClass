// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "OrbArenaCharacter.generated.h"

UENUM()
enum class EGameState : uint8
{
	Playing,
	Won,
	Lost
};

/*
 * Advance Task
 */
UENUM()
enum class EGameDifficulty : uint8
{
	Easy,
	Medium,
	Hard
};

UCLASS()
class ORBARENA_API AOrbArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOrbArenaCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*
	 * Player Variables
	 */
	UPROPERTY(EditAnywhere, Category = "Player|Info")
	FString PlayerName = "Player";

	UPROPERTY(EditAnywhere, Category = "Player|Info")
	int32 PlayerLevel = 1;

	UPROPERTY(EditAnywhere, Category = "Player|Info")
	int32 PlayerLives = 5;

	UPROPERTY(EditAnywhere, Category = "Player|Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player|Stats")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player|Stats")
	float BaseMovementSpeed = 500.f;

	/*
	 * System Variables
	 */
	UPROPERTY(EditAnywhere, Category = "System")
	int32 Score = 0;

	UPROPERTY(EditAnywhere, Category = "System")
	int32 ScoreWinCondition = 50000;

	UPROPERTY(EditAnywhere, Category = "System")
	EGameState GameState = EGameState::Playing;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float MovementMultiplierEasy = 1.f;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float MovementMultiplierMedium = 0.75f;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float MovementMultiplierHard = 0.5f;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float CurrentMovementSpeed = 500.f;

	/*
	* Advance Task
	 */
	UPROPERTY(EditAnywhere, Category = "System")
	EGameDifficulty GameDifficulty = EGameDifficulty::Easy;

	// Temp Variables
	UPROPERTY(EditAnywhere, Category = "Player|Temp")
	float TempDamage = 2.f;

	UPROPERTY(EditAnywhere, Category = "Player|Temp")
	float TempHeal = 2.f;

	UPROPERTY(EditAnywhere, Category = "Player|Temp")
	int32 TempPoints = 10;

	UPROPERTY(EditAnywhere, Category = "Player|Temp")
	float SprintMultiplier = 3;

	bool bIsSprinting = false;

	/*
	* Enhanced Input Assets
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* DamageAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* HealAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AddScoreAction;

	/*
	 * Player Functions
	 */
	void MovePlayer(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();

	void UpdateMovementSpeed();

	void DamageInput();
	void HealInput();
	void AddScoreInput();

	/*
	 * Player Functions
	*/
	void AddScore(int32 InAmount);
	void HealPlayer(float InAmount);
	void ApplyDamageAmount(float InAmount);
	void LoseLife();

	/*
	* System Functions
	*/
	void PrintTextToScreen();
	void CheckGameState();
	void PrintPlayerLocation();

	// void CheckKeyPressed();
	// void MovePlayer(float DeltaTime);
	// void PlayerSprint();
};
