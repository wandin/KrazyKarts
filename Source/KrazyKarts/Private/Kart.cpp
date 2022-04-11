// Fill out your copyright notice in the Description page of Project Settings.


#include "Kart.h"


#include "Components/InputComponent.h"


// Sets default values
AKart::AKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKart::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKart::MoveForward(float Val)
{
	// throttle is the amount of (InputAxis Val)
	Throttle = Val;

}

// Called every frame
void AKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// Force is equal to Actor's direction * by the Force applied to the car when throttle is fully pressed (N) * throttle
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	Acceleration = Force / Mass; // acceleration is the force divided by vehicle's mass, same as (Force = mass * acceleration)
	// update location of Pawn based on velocity applied, it's multiplied by 100 to convert from meters to cm.
	Velocity = Velocity + Acceleration * DeltaTime;
	FVector Translation = Velocity * 100 * DeltaTime;
	AddActorWorldOffset(Translation);

}

// Called to bind functionality to input
void AKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AKart::MoveForward);

}

