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

	void MoveForward(float Val); /** Handle pressing Forward */
	void MoveRight(float Val); /** Handle pressing right */

	// Replicates MoveForward to from server to client
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Val);
	// Replicates MoveRight to from server to client
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Val);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

private:

	FVector GetAirResistance();
	FVector GetRollinResistance();

	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotation(float DeltaTime);

	// mass of the car (Kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000.0f;

	// Force applied to the car when throttle is fully pressed (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// Minimum radius of the car turning circle at full lock (meters)
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	/* Higher value means more drag */
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	/* Higher value means more rolling resistance */
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015f;

	FVector Acceleration;
	FVector Velocity;
			

	UPROPERTY(Replicated)
	FVector ReplicatedLocation;

	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;


	float Throttle;
	float Steering;
};
