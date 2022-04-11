// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Kart.generated.h"



UCLASS()
class KRAZYKARTS_API AKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void MoveForward(float Val);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

private:

	void UpdateLocationFromVelocity(float DeltaTime);

	// mass of the car (Kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000.0f;

	// Force applied to the car when throttle is fully pressed (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	FVector Acceleration;
	FVector Velocity;

	float Throttle;
};
