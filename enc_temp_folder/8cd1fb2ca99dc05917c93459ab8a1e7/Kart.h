// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "Kart.generated.h"

USTRUCT() 
struct FKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float Steering;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FKartMove LastMove;
};


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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

private:

	void SimulateMove(FKartMove Move);

	FKartMove CreateMove(float DeltaTime);
	void ClearAcknowledgeMoves(FKartMove LastMove);

	FVector GetAirResistance();
	FVector GetRollinResistance();

	void ApplyRotation(float DeltaTime, float Steering);

	void UpdateLocationFromVelocity(float DeltaTime);

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

	void MoveForward(float Val); /** Handle pressing Forward */
	void MoveRight(float Val); /** Handle pressing right */

	// Replicates MoveForward to from server to client
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FKartMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FKartState ServerState;
	FVector Acceleration;
	FVector Velocity;


	UFUNCTION()
	void OnRep_ServerState();

	float ThrottleInput;
	float SteeringInput;

	TArray<FKartMove> UnacknowledgeMoves;
};
