// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "KartMovementComponent.h"
#include "KartReplicatorComponent.h"

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
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UKartMovementComponent* MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UKartReplicatorComponent* MovementReplicator;

private:

	void MoveForward(float Val); /** Handle pressing Forward */
	void MoveRight(float Val); /** Handle pressing right */

};
