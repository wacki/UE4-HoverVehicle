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
		::DrawDebugLine(World, Start, HitResult.ImpactPoint, TraceHitColor, false, 0.0f, 0, 2.0f);
		::DrawDebugLine(World, HitResult.ImpactPoint, End, TraceHitColor, false, 0.0f, 0, 2.0f);
		::DrawDebugPoint(World, HitResult.ImpactPoint, 16, TraceHitColor, false, 0.0f);
	}
	else {
		// no hit means all red
		::DrawDebugLine(World, Start, End, TraceColor, false, 0.0f, 0, 2.0f);
	}
}


// Sets default values
AHoverVehicleBase::AHoverVehicleBase()
	: VehicleMesh(nullptr), 
	HoverDistance(100.0f),
	HoverDampening(0.4f),
	UpOrientationOffset(FQuat::Identity)
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


	// Testwise friction stuff
	CalculateFrictionForce();
}


void AHoverVehicleBase::UpdateHoverForce(float DeltaTime)
{
	// Calculate trace destination
	FVector TraceEnd;
	FVector WorldPos = GetActorLocation();
	FVector UpDir = GetActorUpVector();
	float TraceDistance = HoverDistance;

	// double trace distance if we're magnetic
	TraceDistance *= 2.0f;

	TraceEnd = WorldPos - UpOrientationOffset * UpDir * TraceDistance;


	// do a line trace along our down vector
	FHitResult OutHit = FHitResult();
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, WorldPos, TraceEnd, ECC_Visibility);


	// temp debug draw
	DrawDebugLinetrace(bHit, OutHit, GetWorld(), WorldPos, TraceEnd);

	IsOnGround = bHit;

	// Early out, no hovering needed
	if(!bHit)
		return;

	// update spring velocity
	float HitPointDelta = (OutHit.Location - WorldPos).Size();
	float HoverSpringDelta = HoverDistance - HitPointDelta;
	HoverSpringVelocity = (PrevHoverSpringDelta - HoverSpringDelta) / DeltaTime;

	float HoverForce = CalculateSpringForce(HoverSpringDelta, DeltaTime, HoverSpringVelocity);
	if(!IsMagnetic)
		HoverForce = FMath::Clamp(HoverForce, 0.0f, 99999999999.0f);

	FVector HoverForceVec = OutHit.Normal * HoverForce;


	VehicleMesh->AddForce(HoverForceVec);

	// update prev spring delta
	PrevHoverSpringDelta = HoverSpringDelta;

	// Update goal up vector
	GoalUpVector = OutHit.ImpactNormal;
}

void AHoverVehicleBase::UpdateSelfRightingTorque(float DeltaTime)
{
	// calculate delta rotaion towards goal up vector
	FVector UpDir = UpOrientationOffset * GetActorUpVector();

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

	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Torque.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::SanitizeFloat(OrientationSpringVelocity));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

	/*float SpringRatio = RestPositionDelta / HoverDistance;
	return (SpringRatio * SpringStiffness - HoverDampening * Velocity);*/


	//FRotator DeltaRotator = FRotator()
}

// Called to bind functionality to input
void AHoverVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);


	//PlayerInputComponent->BindAxis("TurnRate", this, &AHoverVehicleBase::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AHoverVehicleBase::LookUpAtRate);
}


void AHoverVehicleBase::CalculateFrictionForce()
{
	if(!IsOnGround)
		return;

	// relative velocity
	FVector Velocity = VehicleMesh->GetPhysicsLinearVelocity();
	Velocity.VectorPlaneProject(Velocity, CurrentGroundNormal);

	FVector Start = GetActorLocation();
	FVector Dir = Velocity;
	Dir.Normalize();
	FVector End = Start + Dir * 1000;

	::DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 0.0f, 0, 3.0f);

	// convert to local space
	Velocity = GetActorRotation().UnrotateVector(Velocity);

	// apply sideways and forward friction
	// for now we just completely remove sideways velocity
	Velocity.X = 0;
	Velocity.Y *= -1;

	Velocity = GetActorRotation().RotateVector(Velocity);

	// Add the converted velocity as an impulse
	VehicleMesh->AddImpulse(Velocity);

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
	// Testwise implementation of a more controllable spring equation maybe?
	// source: https://www.gamedev.net/resources/_/technical/math-and-physics/towards-a-simpler-stiffer-and-more-stable-spring-r3227
	//F = -m / (DeltaTime*DeltaTime) * Ck * x - m / DeltaTime * Cd * v

	float Mass = VehicleMesh->GetMass();
	float Ck = 0.05f;
	float Cd = 0.1f;

	return Mass / (DeltaTime*DeltaTime) * Ck * RestPositionDelta - Mass / DeltaTime * Cd * Velocity;

	/**/
}