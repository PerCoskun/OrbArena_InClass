// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbArenaProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "OrbArena/Characters/OrbArenaEnemy.h"


AOrbArenaProjectile::AOrbArenaProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(RootComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileComponent");
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>("CollisionSphere");
	SphereComponent->SetupAttachment(RootComponent);
}

void AOrbArenaProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AOrbArenaProjectile::DamageEnemy);
}

void AOrbArenaProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TimeToSelfDestroy = FMath::Max(TimeToSelfDestroy - DeltaTime, 0.f);
	
	UE_LOG(LogHAL, Log, TEXT("DestroyTimer: %f"), TimeToSelfDestroy);
	
	if (TimeToSelfDestroy <= 0.f)
	{
		UE_LOG(LogHAL, Log, TEXT("DESTROYED"));
		Destroy();
	}
}

void AOrbArenaProjectile::DamageEnemy(AActor* ThisActor, AActor* OtherActor)
{
	// UE_LOG(LogHAL, Log, TEXT("Has hit: %s"), *OtherActor->GetName());
	
	AOrbArenaEnemy* Enemy = Cast<AOrbArenaEnemy>(OtherActor);
	
	if (IsValid(Enemy))
	{
		UE_LOG(LogHAL, Log, TEXT("Has hit: %s"), *Enemy->GetName());

		Enemy->ApplyDamageAmount(ProjectileDamage);
		Destroy();
	}
}