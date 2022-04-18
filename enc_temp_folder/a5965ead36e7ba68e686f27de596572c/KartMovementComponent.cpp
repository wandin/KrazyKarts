// Fill out your copyright notice in the Description page of Project Settings.


#include "KartMovementComponent.h"

#include "GameFramework/GameStateBase.h"

// Sets default values for this component's properties
UKartMovementComponent::UKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}

}

void UKartMovementComponent::SimulateMove(const FKartMove& Move)
{
	// Force is equal to Actor's direction * by the Force applied to the car when throttle is fully pressed (N) * throttle
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	Force += GetAirResistance();
	Force += GetRollinResistance();

	// acceleration is the force divided by vehicle's mass, same as (Force = mass * acceleration)
	Acceleration = Force / Mass;

	// update location of Pawn based on velocity applied, it's multiplied by 100 to convert from meters to cm.
	Velocity = Velocity + Acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.Steering);
	UpdateLocationFromVelocity(Move.DeltaTime);
}

FKartMove UKartMovementComponent::CreateMove(float DeltaTime)
{
	FKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.Steering = SteeringInput;
	Move.Throttle = ThrottleInput;
	Move.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds(); // time that will be helpful to measure which move was created first
	// instead of using GetWorld()->GetTimeSeconds(), the approach used is more robust thich tries to guess the lag and give the current server time
	return Move;
}

FVector UKartMovementComponent::GetAirResistance()
{
	// returns negative because it's against the car's velocity direction
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector UKartMovementComponent::GetRollinResistance()
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;

	// returns negative because it's against the car's force direction
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void UKartMovementComponent::ApplyRotation(float DeltaTime, float Steering)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * Steering;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);
	GetOwner()->AddActorWorldRotation(RotationDelta);
}

void UKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;

	FHitResult Hit;
	GetOwner()->AddActorWorldOffset(Translation, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

