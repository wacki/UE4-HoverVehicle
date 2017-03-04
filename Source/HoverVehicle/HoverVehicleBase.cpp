// Fill out your copyright notice in the Description page of Project Settings.

#include "HoverVehicle.h"
#include "HoverVehicleBase.h"


////////////////////////////////////////////
// Temp debug draw function for traces /////
// todo:	put this somewhere else. Maybe make a debug util class
//			maybe make a helper class for traces
// this is lifted directly from the blueprint trace by channel function
void DrawDebugLinetrace(bool bHit, const FHitResult& HitResult, const UWorld* World, const FVector& Start, const FVector& End)
{
	FColor TraceColor = FColor::Red;
	FColor TraceHitColor = FColor::Green;

	// @fixme, draw line with thickness = 2.f?
	if(bHit && HitResult.bBlockingHit) {
		// Red up to the blocking hit, green thereafter
		::DrawDebugLine(World, Start, HitResult.ImpactPoint, TraceHitColor, false, 0.0f);
		::DrawDebugLine(World, HitResult.ImpactPoint, End, TraceHitColor, false, 0.0f);
		::DrawDebugPoint(World, HitResult.ImpactPoint, 16, TraceHitColor, false, 0.0f);
	}
	else {
		// no hit means all red
		::DrawDebugLine(World, Start, End, TraceColor, false, 0.0f);
	}
}


// Sets default values
AHoverVehicleBase::AHoverVehicleBase()
	: VehicleMesh(nullptr), 
	HoverDistance(100.0f),
	HoverDampening(0.4f)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHoverVehicleBase::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AHoverVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Calculate and set hover force
	UpdateHoverForce(DeltaTime);

	// Update self righting torque
	UpdateSelfRightingTorque(DeltaTime);

}


void AHoverVehicleBase::UpdateHoverForce(float DeltaTime)
{
	// Calculate trace destination
	FVector TraceEnd;
	FVector WorldPos = GetActorLocation();
	FVector UpDir = GetActorUpVector();
	float TraceDistance = HoverDistance;

	// double trace distance if we're magnetic
	if(IsMagnetic)
		TraceDistance *= 2.0f;

	TraceEnd = WorldPos - UpDir * TraceDistance;


	// do a line trace along our down vector
	FHitResult OutHit = FHitResult();
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, WorldPos, TraceEnd, ECC_Visibility);

	// temp debug draw
	DrawDebugLinetrace(bHit, OutHit, GetWorld(), WorldPos, TraceEnd);

	// Early out, no hovering needed
	if(!bHit)
		return;

	// update spring velocity
	float HitPointDelta = (OutHit.Location - WorldPos).Size();
	float HoverSpringDelta = HoverDistance - HitPointDelta;
	HoverSpringVelocity = (PrevHoverSpringDelta - HoverSpringDelta) / DeltaTime;

	FVector HoverForceVec = OutHit.Normal * CalculateSpringForce(HoverSpringDelta, DeltaTime, HoverSpringVelocity);


	VehicleMesh->AddForce(HoverForceVec);

	// update prev spring delta
	PrevHoverSpringDelta = HoverSpringDelta;

	// Update goal up vector
	GoalUpVector = OutHit.ImpactNormal;
}

void AHoverVehicleBase::UpdateSelfRightingTorque(float DeltaTime)
{
	// calculate delta rotaion towards goal up vector
	FVector UpDir = GetActorUpVector();

	// 
	FQuat DeltaRot = FQuat::FindBetweenNormals(UpDir, GoalUpVector);
	float Angle;
	FVector Axis;
	DeltaRot.ToAxisAndAngle(Axis, Angle);


	//  F = -k(|x|-d)(x/|x|) - bv
	float TempK = 300000.0f;
	float TempD = 100000.0f;

	FVector AngularVelocity = VehicleMesh->GetPhysicsAngularVelocity();

	float RestDelta = Angle;
	float OrientationSpringDelta = Angle;
	OrientationSpringVelocity = (PrevOrientationSpringDelta - OrientationSpringDelta) / DeltaTime;


	FVector Torque = -TempK * DeltaRot.Euler() - TempD * AngularVelocity;
	Torque.Z = 0;
	VehicleMesh->AddTorque(Torque);
	PrevOrientationSpringDelta = OrientationSpringDelta;

	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Torque.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::SanitizeFloat(OrientationSpringVelocity));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

	/*float SpringRatio = RestPositionDelta / HoverDistance;
	return (SpringRatio * SpringStiffness - HoverDampening * Velocity);*/


	//FRotator DeltaRotator = FRotator()
}

// Called to bind functionality to input
void AHoverVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Orientate the vehicle 
void AHoverVehicleBase::SetUpOrientation(const FVector& Dir)
{
}


FVector AHoverVehicleBase::CalculateFrictionForce()
{
	return FVector();
}

// todo: revisit this calculation. It works but it seems off...
float AHoverVehicleBase::CalculateSpringForce(float RestPositionDelta, float DeltaTime, float Velocity)
{
	/**/
	// calculate and apply damped spring hover force
	float SpringRatio = RestPositionDelta / HoverDistance;
	return (SpringRatio * SpringStiffness - HoverDampening * Velocity);

	/**/


	/*/
	// Testwise implementation of a more controllable spring equation
	// source: https://www.gamedev.net/resources/_/technical/math-and-physics/towards-a-simpler-stiffer-and-more-stable-spring-r3227
	//F = -m / (DeltaTime*DeltaTime) * Ck * x - m / DeltaTime * Cd * v

	float Mass = VehicleMesh->GetMass();
	float Ck = 0.05f;
	float Cd = 0.1f;

	return Mass / (DeltaTime*DeltaTime) * Ck * RestPositionDelta - Mass / DeltaTime * Cd * Velocity;

	/**/
}