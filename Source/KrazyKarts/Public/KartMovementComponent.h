// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KartMovementComponent.generated.h"


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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FKartMove& Move);

	FVector GetVelocity() { return Velocity; };

	void SetVelocity(FVector Value) { Velocity = Value; };

	void SetThrottle(float Value) { ThrottleInput = Value; };
	void SetSteering(float Value) { SteeringInput = Value; };

	FKartMove GetLastMove() { return LastMove; };

	
private:

	FKartMove CreateMove(float DeltaTime);

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


	FVector Acceleration;
	FVector Velocity;


	float ThrottleInput;
	float SteeringInput;

	FKartMove LastMove;
};
