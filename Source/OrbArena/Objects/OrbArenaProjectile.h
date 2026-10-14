// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbArenaProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class ORBARENA_API AOrbArenaProjectile : public AActor
{
	GENERATED_BODY()

public:
	AOrbArenaProjectile();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void DamageEnemy(AActor* ThisActor, AActor* OtherActor);
	
	/*
	 * Data
	 */
	UPROPERTY(EditAnywhere, Category = "Projectile|Data")
	float Damage = 10.f;
	
	/*
	 * Components
	 */
	UPROPERTY(EditAnywhere, Category = "Projectile|Component")
	UStaticMeshComponent*  StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Component")
	USphereComponent* SphereComponent;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Component")
	UProjectileMovementComponent* ProjectileMovementComponent;
};
