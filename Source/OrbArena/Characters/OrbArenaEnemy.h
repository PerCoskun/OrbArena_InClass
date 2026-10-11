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
	float MovementSpeed = 500.f;

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
	
	UPROPERTY(EditAnywhere, Category = "Enemy|Info")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, Category = "Enemy|Stats")
	FEnemyStats EnemyStats;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Attack")
	float AttackCooldownRemaining = 0.f;
	
	// Helpers
	UPROPERTY()
	AOrbArenaCharacter* PlayerCharacter = nullptr;
	
	void AttackPlayer();
	void ApplyDamageAmount(float InAmount);
	void Debugging();
};
