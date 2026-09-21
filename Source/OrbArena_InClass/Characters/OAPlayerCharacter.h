// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OAPlayerCharacter.generated.h"

UCLASS()
class ORBARENA_INCLASS_API AOAPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOAPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category="Player")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category="Player")
	float MovementSpeed = 350.f;
	
	UPROPERTY(EditAnywhere, Category="Player|Info")
	FString PlayerName = "Tor" ;
	
};
