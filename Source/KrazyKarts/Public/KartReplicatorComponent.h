// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KartMovementComponent.h"
#include "KartReplicatorComponent.generated.h"

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

struct FHermiteCubicSpline
{
	FVector StartLocation, StartDerivative, TargetLocation, TargetDerivative;

	FVector InterpolateLocation(float LerpRatio) const
	{
		return FMath::CubicInterp(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
	}
	FVector InterpolateDerivative(float LerpRatio) const
	{
		return FMath::CubicInterpDerivative(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UKartReplicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKartReplicatorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:

	void ClearAcknowledgeMoves(FKartMove LastMove);

	void UpdateServerState(const FKartMove& Move);
	
	void ClientTick(float DeltaTime);

	FHermiteCubicSpline CreateSpline();

	void InterpolateLocation(const FHermiteCubicSpline &Spline, float LerpRatio);
	void InterpolateVelocity(const FHermiteCubicSpline& Spline, float LerpRatio);
	void InterpolateRotation(float LerpRatio);
	float VelocityToDerivative();

	// Replicates MoveForward to from server to client
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FKartMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	void AutonomousProxy_OnRep_ServerState();
	void SimulatedProxy_OnRep_ServerState();

	TArray<FKartMove> UnacknowledgeMoves;

	float ClientTimeSinceUpdate;
	float ClientTimeBetweenLastUpdates;
	FTransform ClientStartTransform;
	FVector ClientStartVelocity;

	float ClientSimulatedTime;

	UPROPERTY()
	UKartMovementComponent* MovementComponent;

	UPROPERTY()
	USceneComponent* MeshOffsetRoot;

	UFUNCTION(BlueprintCallable)
		void SetMeshOffsetRoot(USceneComponent* Root)
	{
		MeshOffsetRoot = Root;
	}
};
