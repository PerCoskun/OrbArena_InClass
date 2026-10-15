// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "OrbArenaCharacter.generated.h"

class AOrbArenaProjectile;
class AOrbArenaEnergyOrb;

UENUM()
enum class EGameState : uint8
{
	Playing,
	Won,
	Lost
};

UENUM()
enum class EGameDifficulty : uint8
{
	Easy,
	Medium,
	Hard
};

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MovementSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Damage = 10.f;
};

UCLASS()
class ORBARENA_API AOrbArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOrbArenaCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

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
	FPlayerStats PlayerStats;

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
	EGameDifficulty GameDifficulty = EGameDifficulty::Easy;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float MovementMultiplierEasy = 1.f;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float MovementMultiplierMedium = 0.75f;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float MovementMultiplierHard = 0.5f;

	UPROPERTY(EditAnywhere, Category = "System|Difficulty")
	float CurrentMovementSpeed = 500.f;

	/*
	 * Orb Loot
	 */
	UPROPERTY(VisibleAnywhere, Category = "Orbs")
	TArray<AOrbArenaEnergyOrb*> EnergyOrbArray;

	UPROPERTY(VisibleAnywhere, Category = "Orbs")
	int32 OrbsRemaining = 0;

	UPROPERTY(EditAnywhere, Category = "Orbs")
	bool bWinWhenAllOrbsCollected = false;

	// EXERCISES - USE AS EXAMPLES ----------------------------------- //
	// UPROPERTY(EditAnywhere, Category = "Exercises|Arrays")
	// TArray<int32> ExampleScores;
	//
	// UPROPERTY(EditAnywhere, Category = "Exercises|Arrays")
	// TArray<FString> ExampleOrbNames;
	// 
	// UPROPERTY(EditAnywhere, Category = "Exercises|Arrays")
	// FString OrbNameToFind = TEXT("Green Orb");
	// UPROPERTY(EditAnywhere, Category = "Orbs|Loot")
	// TArray<FLootDrop> LootDrops;
	//----------------


	// Helpers
	int32 StartingOrbCount = 0;
	bool bAllOrbsCollected = false;
	
	UPROPERTY()
	APlayerController* ArenaPlayerController;
	
	/*
	 * Combat
	 */
	UPROPERTY(EditAnywhere, Category = "Player|Combat")
	TSubclassOf<AOrbArenaProjectile> ProjectileBlueprint;

	UPROPERTY(EditAnywhere, Category = "Player|Combat")
	float FireCooldown = 0.5f;

	UPROPERTY(VisibleAnywhere, Category = "Player|Combat")
	float RemainingFireCooldown = 0.f;

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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* DamageEnemyAction;

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
	void DamageNearestEnemyInput();
		
	void ShootProjectile();

	/*
	* System Functions
	*/
	void AddScore(int32 InAmount);
	void HealPlayer(float InAmount);
	void ApplyDamageAmount(float InAmount);
	void LoseLife();

	void FindEnergyOrbs();
	void UpdateRemainingOrbCount();

	void PrintLootDrops();
	void PrintTextToScreen();
	void CheckGameState();
	void PrintPlayerLocation();

	// EXERCISES - USE AS EXAMPLES ----------------------------------- //
	// void RunArrayExercises();
	// void PrintStructExercise();
	// FLootDrop GetHighestValueLootDrop();
};
