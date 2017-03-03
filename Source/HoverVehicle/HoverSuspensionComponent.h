// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "HoverSuspensionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class HOVERVEHICLE_API UHoverSuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
	float HoverDistance;

	UPROPERTY(EditAnywhere)
	float SpringStiffness;

	UPROPERTY(EditAnywhere)
	bool IsMagnetic;
	
	UPROPERTY(EditAnywhere)
	float HoverDampening;

	UPROPERTY(EditAnywhere)
	float SidewaysFriction;
	
	UPROPERTY(EditAnywhere)
	float ForwardFriction;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TargetBody;
	

	// Sets default values for this component's properties
	UHoverSuspensionComponent();


protected:
	float DistanceFromSurface;
	float PrevDistanceFromSurface;
	float SpringVelocity;

	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateSpringVelocity(const FVector& ImpactLocation, float DeltaSeconds);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetTargetBody(UStaticMeshComponent* body);
	
};
