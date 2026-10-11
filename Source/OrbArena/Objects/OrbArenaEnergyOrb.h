// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbArenaEnergyOrb.generated.h"

class AOrbArenaCharacter;

UENUM(BlueprintType)
enum class ELootRarity : uint8
{
	Common,
	Rare,
	Epic
};

USTRUCT(BlueprintType)
struct FLootDrop
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FString ItemName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	ELootRarity Rarity = ELootRarity::Common;
};

UCLASS()
class ORBARENA_API AOrbArenaEnergyOrb : public AActor
{
	GENERATED_BODY()

public:
	AOrbArenaEnergyOrb();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	/*
	 * Orb attribute variables
	 */
	UPROPERTY(EditAnywhere, Category = "Config")
	float PickupRadius = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	FLootDrop OrbLootDrop;

	UPROPERTY(EditAnywhere, Category = "Config")
	FColor OrbDebugColor = FColor::Emerald;
	
	UPROPERTY(VisibleAnywhere, Category = "Config")
	bool bHasBeenPickedUp = false;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	float OrbRotationSpeed = 120.f;
	
	/*
	 * Helper Functions
	 */
	UPROPERTY()
	AOrbArenaCharacter* PlayerCharacter;
	
	void OrbPickedUp(int32 InPointWorth);
	
};
