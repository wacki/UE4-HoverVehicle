// Fill out your copyright notice in the Description page of Project Settings.

#include "HoverVehicle.h"
#include "HoverSuspensionComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values for this component's properties
UHoverSuspensionComponent::UHoverSuspensionComponent()
	: IsMagnetic(false), TargetBody(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHoverSuspensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UHoverSuspensionComponent::UpdateSpringVelocity(const FVector& ImpactLocation, float DeltaSeconds)
{

	FVector ImpactDelta = ImpactLocation - GetComponentLocation();
	DistanceFromSurface = HoverDistance - ImpactDelta.Size();

	SpringVelocity = (PrevDistanceFromSurface - DistanceFromSurface) / DeltaSeconds;
}


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


// Called every frame
void UHoverSuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Calculate trace destination

	FVector TraceEnd;
	FVector WorldPos = GetComponentLocation();
	FVector UpDir = GetUpVector();
	float TraceDistance = HoverDistance;

	// double trace distance if we're magnetic
	if(IsMagnetic)
		TraceDistance *= 2;

	TraceEnd = WorldPos - UpDir * TraceDistance;


	// do a line trace along our down vector
	FHitResult OutHit = FHitResult();	
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, WorldPos, TraceEnd, ECC_Visibility);

	DrawDebugLinetrace(bHit, OutHit, GetWorld(), WorldPos, TraceEnd);

	if(!bHit)
		return;

	UpdateSpringVelocity(OutHit.Location, DeltaTime);

	// calculate and apply damped spring hover force
	float SpringRatio = DistanceFromSurface / HoverDistance;

	FVector HoverForceVec = UpDir * (SpringRatio * SpringStiffness - HoverDampening * SpringVelocity);

	TargetBody->AddForceAtLocation(HoverForceVec, WorldPos);

	PrevDistanceFromSurface = DistanceFromSurface;

	// Debug vis
	//DrawDebugLine(GetWorld(), HitResult.Location, HitResult.Location + HitResult.Normal * 100.0f, FColor::Blue, false, 10.0f, 0, 5.0f);

}

void UHoverSuspensionComponent::SetTargetBody(UStaticMeshComponent* body)
{
	TargetBody = body;
}