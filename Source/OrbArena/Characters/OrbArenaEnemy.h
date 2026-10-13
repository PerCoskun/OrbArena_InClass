// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OrbArenaEnemy.generated.h"

class AOrbArenaCharacter;

USTRUCT(BlueprintType)
struct FEnemyStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MovementSpeed = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackRange = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackCooldown = 2.f;
};

UCLASS()
class ORBARENA_API AOrbArenaEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AOrbArenaEnemy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/*
	 * Enemy Variables
	 */
	UPROPERTY(EditAnywhere, Category = "Enemy|Info")
	FString EnemyName = "Enemy";

	UPROPERTY(EditAnywhere, Category = "Enemy|Stats")
	FEnemyStats EnemyStats;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Attack")
	float AttackCooldownRemaining = 0.f;

	/*
	 * AI Variables
	 */
	UPROPERTY(EditAnywhere, Category = "Enemy|Vision",		meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float DotProductValueToSeePlayer = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Enemy|Vision")
	float DotProductValueToStopTurning = 0.9f;
	
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Vision")
	float DotProductValue = 0.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Vision")
	FVector CrossProductValue = FVector(0.f, 0.f, 0.f);
	
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Vision")
	bool bCanSeePlayer = false;

	UPROPERTY(EditAnywhere, Category = "Enemy|Turning")
	float TurnSpeed = 90.f;

	UPROPERTY(EditAnywhere, Category = "Enemy|Turning")
	bool bRequireVisionToTurn = true;

	UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
	bool bMoveTowardsPlayer = false;
	
	/*
	 * Debugging
	 */
	UPROPERTY(EditAnywhere, Category = "Enemy|Debug")
	bool bLogDotCrossProduct = true;
	
	UPROPERTY(EditAnywhere, Category = "Enemy|Info")
	bool bDrawDebug = true;
	
	// Helpers
	UPROPERTY()
	AOrbArenaCharacter* PlayerCharacter = nullptr;

	/*
	* Functions
	*/
	void AttackPlayer();
	void ApplyDamageAmount(float InAmount);
	void Debugging();
	
	void UpdateVisionAndTurning(float DeltaTime);
	void MoveTowardsPlayer(float DeltaTime);
};
