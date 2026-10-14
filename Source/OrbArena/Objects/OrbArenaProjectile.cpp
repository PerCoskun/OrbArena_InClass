// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "OrbArena/Characters/OrbArenaEnemy.h"


AOrbArenaProjectile::AOrbArenaProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(RootComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SphereComponent->SetupAttachment(RootComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

void AOrbArenaProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AOrbArenaProjectile::DamageEnemy);
}

void AOrbArenaProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOrbArenaProjectile::DamageEnemy(AActor* ThisActor, AActor* OtherActor)
{
	AOrbArenaEnemy* Enemy = Cast<AOrbArenaEnemy>(OtherActor);
	
	if (IsValid(Enemy))
	{
		Enemy->ApplyDamageAmount(Damage);
	}
}