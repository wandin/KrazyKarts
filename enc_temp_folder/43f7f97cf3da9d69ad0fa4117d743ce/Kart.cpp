// Fill out your copyright notice in the Description page of Project Settings.


#include "Kart.h"

#include "Components/InputComponent.h"


#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetWork.h"


#include "GameFramework/GameStateBase.h"

// Sets default values
AKart::AKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

}

// Called when the game starts or when spawned
void AKart::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
}

void AKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKart, ServerState);
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";

	}
}

// Called every frame
void AKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		FKartMove Move = CreateMove(DeltaTime);


		if (!HasAuthority())
		{
			UnacknowledgeMoves.Add(Move);

			UE_LOG(LogTemp, Warning, TEXT("Queue length: %d"), UnacknowledgeMoves.Num());
		}

		Server_SendMove(Move);
		
		SimulateMove(Move);
	}

	DrawDebugString(GetWorld(), FVector(0,0,100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}

void AKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;

	ClearAcknowledgeMoves(ServerState.LastMove);
}

void AKart::SimulateMove(FKartMove Move)
{

	// Force is equal to Actor's direction * by the Force applied to the car when throttle is fully pressed (N) * throttle
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	Force += GetAirResistance();
	Force += GetRollinResistance();

	// acceleration is the force divided by vehicle's mass, same as (Force = mass * acceleration)
	Acceleration = Force / Mass;

	// update location of Pawn based on velocity applied, it's multiplied by 100 to convert from meters to cm.
	Velocity = Velocity + Acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.Steering);

	UpdateLocationFromVelocity(Move.DeltaTime);

}

FKartMove AKart::CreateMove(float DeltaTime)
{
	FKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.Steering = SteeringInput;
	Move.Throttle = ThrottleInput;
	Move.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds(); // time that will be helpful to measure which move was created first
	// instead of using GetWorld()->GetTimeSeconds(), the approach used is more robust thich tries to guess the lag and give the current server time

	return Move;
}

void AKart::ClearAcknowledgeMoves(FKartMove LastMove)
{
	// iterating throught the array to find old 'moves' to clear
	TArray<FKartMove> NewMoves;

	for (const FKartMove& Move : UnacknowledgeMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgeMoves = NewMoves;
}

FVector AKart::GetAirResistance()
{
	// returns negative because it's against the car's velocity direction
	return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AKart::GetRollinResistance()
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;

	// returns negative because it's against the car's force direction
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void AKart::ApplyRotation(float DeltaTime, float Steering)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * Steering;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);
	AddActorWorldRotation(RotationDelta);
}

void AKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;

	FHitResult Hit;
	AddActorWorldOffset(Translation, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

// Called to bind functionality to input
void AKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKart::MoveRight);

}

void AKart::MoveForward(float Val)
{
	ThrottleInput = Val;
}

void AKart::MoveRight(float Val)
{
	SteeringInput = Val;
}

void AKart::Server_SendMove_Implementation(FKartMove Move)
{
	SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;
}

bool AKart::Server_SendMove_Validate(FKartMove Move)
{
	return true; //TODO: Make better validation
}

