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
	
	// Sets default values for this pawn's properties
	AHoverVehicleBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetUpOrientation();	
};
