// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "HoverVehicleBase.generated.h"

UCLASS()
class HOVERVEHICLE_API AHoverVehicleBase : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float HoverDistance;

	UPROPERTY(EditAnywhere)
	bool IsMagnetic;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* VehicleMesh;

	UPROPERTY(EditAnywhere)
	float SpringStiffness;

	UPROPERTY(EditAnywhere)
	float HoverDampening;

	UPROPERTY(EditAnywhere)
	FVector GoalUpVector;

	// Sets default values for this pawn's properties
	AHoverVehicleBase();

	UFUNCTION(BlueprintCallable)
	void SetVehicleMesh(UStaticMeshComponent* Mesh) { VehicleMesh = Mesh; }

private:
	float HoverSpringVelocity;
	float PrevHoverSpringDelta;

	float OrientationSpringVelocity;
	float PrevOrientationSpringDelta;

	FVector CurrentGroundNormal;
	FQuat UpOrientationOffset;

	bool IsOnGround;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float CalculateSpringForce(float RestPositionDelta, float DeltaTime, float Velocity);
	
	virtual void CalculateFrictionForce();

	virtual void UpdateHoverForce(float DeltaTime);

	virtual void UpdateSelfRightingTorque(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetUpOrientationOffset(const FRotator& Offset) {
		UpOrientationOffset = FQuat(Offset);
	}
};
