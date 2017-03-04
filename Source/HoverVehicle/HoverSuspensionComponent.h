// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "HoverSuspensionComponent.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EHoverSuspensionForceDirEnum : uint8 {
	HFD_SuspensionUp 	UMETA(DisplayName = "SuspensionUp"),
	HFD_SurfaceNormal 	UMETA(DisplayName = "SurfaceNormal")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class HOVERVEHICLE_API UHoverSuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere)
	EHoverSuspensionForceDirEnum ForceDirEnum;

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

	void UpdateSpringVelocity(const FVector& ImpactLocation, float DeltaSeconds);


protected:
	float DistanceFromSurface;
	float PrevDistanceFromSurface;
	float SpringVelocity;

	// Called when the game starts
	virtual void BeginPlay() override;


	virtual float CalculateSpringForce(float DeltaTime, const FVector& HitLocation);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetTargetBody(UStaticMeshComponent* body);
	
	UFUNCTION(BlueprintCallable)
	virtual void SetHoverDistance(float Distance) { HoverDistance = Distance; }
};
