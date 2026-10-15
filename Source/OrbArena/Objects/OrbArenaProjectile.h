// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbArenaProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ORBARENA_API AOrbArenaProjectile : public AActor
{
	GENERATED_BODY()

public:
	AOrbArenaProjectile();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	/*
	 * Components
	 */
	UPROPERTY(EditAnywhere, Category = "Projectile|Component")
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Component")
	USphereComponent* SphereComponent;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Component")
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	/*
	 * Data
	 */
	UPROPERTY(EditAnywhere, Category = "Projectile|Data")
	float ProjectileDamage = 10.f;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Data")
	float TimeToSelfDestroy = 2.f;

	/*
	 * Function
	 */
	UFUNCTION()
	void DamageEnemy(AActor* ThisActor, AActor* OtherActor);
};
