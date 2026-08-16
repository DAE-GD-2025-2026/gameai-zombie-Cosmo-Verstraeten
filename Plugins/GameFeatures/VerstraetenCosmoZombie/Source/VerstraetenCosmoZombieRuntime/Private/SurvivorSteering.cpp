#include "SurvivorSteering.h"

#include "GameFramework/Pawn.h"

#include "DrawDebugHelpers.h"

USurvivorSteering::USurvivorSteering()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USurvivorSteering::StartWander()
{
	SteeringMode = ESurvivorSteeringMode::Wander;
	


	WanderAngle =
	  FMath::DegreesToRadians(
		  MovementDirection.Rotation().Yaw);

	SetComponentTickEnabled(true);
}

void USurvivorSteering::StopSteering()
{
	SteeringMode = ESurvivorSteeringMode::None;

	SetComponentTickEnabled(false);
}

void USurvivorSteering::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(
		DeltaTime,
		TickType,
		ThisTickFunction);

	switch (SteeringMode)
	{
	case ESurvivorSteeringMode::Wander:
		UpdateWander(DeltaTime);
		break;

	case ESurvivorSteeringMode::Seek:
		break;

	case ESurvivorSteeringMode::Flee:
		break;

	case ESurvivorSteeringMode::None:
		break;
	}
}

void USurvivorSteering::BeginPlay()
{
	Super::BeginPlay();

	CachedPawn = Cast<APawn>(GetOwner());
	
	MovementDirection = CachedPawn->GetActorForwardVector();

	MovementDirection.Z = 0.f;
	MovementDirection.Normalize();
}

void USurvivorSteering::UpdateWander(float DeltaTime)
{
	if (!CachedPawn)
	{
		return;
	}
	WanderChangeTimer -= DeltaTime;

	if (WanderChangeTimer <= 0.f)
	{
		WanderChangeTimer = WanderChangeInterval;

		WanderAngularVelocity =
			FMath::FRandRange(
				-MaxWanderAngularVelocity,
				MaxWanderAngularVelocity);
	}

	// Move the remembered wander angle gradually.
	WanderAngle +=
		FMath::DegreesToRadians(WanderAngularVelocity)
		* DeltaTime;	


	

	const FVector CircleCenter = CachedPawn->GetActorLocation() + MovementDirection * WanderDistance;


	//const FVector CircleOffset = Forward * FMath::Cos(WanderAngle) * WanderRadius  + Right * FMath::Sin(WanderAngle) * WanderRadius;
	const FVector CircleOffset{
		FMath::Cos(WanderAngle) * WanderRadius,
		FMath::Sin(WanderAngle) * WanderRadius,
		0.f
	};

	const FVector WanderTarget = CircleCenter + CircleOffset;


	FVector DesiredDirection = WanderTarget - CachedPawn->GetActorLocation();

	DesiredDirection.Z = 0.f;
	DesiredDirection.Normalize();

	RotateMoveDirection(DesiredDirection, DeltaTime);

	CachedPawn->AddMovementInput(MovementDirection, 1.f);
	
	const FVector DebugOffset{ 0.f, 0.f, 20.f };

	const FVector DebugCircleCenter =
		CircleCenter + DebugOffset;

	const FVector DebugWanderTarget =
		WanderTarget + DebugOffset;

	const FVector DebugPawnLocation =
		CachedPawn->GetActorLocation() + DebugOffset;


	DrawDebugCircle(
		GetWorld(),
		DebugCircleCenter,
		WanderRadius,
		32,
		FColor::Green,
		false,
		0.f,
		0,
		2.f,
		FVector::ForwardVector,
		FVector::RightVector,
		false
	);


	DrawDebugPoint(
		GetWorld(),
		DebugWanderTarget,
		15.f,
		FColor::Red,
		false,
		0.f);


	DrawDebugLine(
		GetWorld(),
		DebugPawnLocation,
		DebugWanderTarget,
		FColor::Red,
		false,
		0.f,
		0,
		2.f);


	DrawDebugLine(
		GetWorld(),
		DebugPawnLocation,
		DebugPawnLocation + MovementDirection * 150.f,
		FColor::Blue,
		false,
		0.f,
		0,
		3.f);
}

/*void USurvivorSteering::FaceDirection(
	const FVector& DesiredDirection,
	float DeltaTime)
{
	if (!CachedPawn)
	{
		return;
	}

	if (DesiredDirection.IsNearlyZero())
	{
		return;
	}

	FRotator CurrentRotation =
		CachedPawn->GetActorRotation();

	const float TargetYaw =
		DesiredDirection.Rotation().Yaw;

	CurrentRotation.Yaw =
		FMath::FixedTurn(
			CurrentRotation.Yaw,
			TargetYaw,
			MaxAngularVelocity * DeltaTime);

	CurrentRotation.Pitch = 0.f;
	CurrentRotation.Roll = 0.f;

	CachedPawn->SetActorRotation(CurrentRotation);
}*/

void USurvivorSteering::RotateMoveDirection(
	const FVector& DesiredDirection,
	float DeltaTime)
{
	if (DesiredDirection.IsNearlyZero())
	{
		return;
	}

	const float CurrentYaw =
		MovementDirection.Rotation().Yaw;

	const float TargetYaw =
		DesiredDirection.Rotation().Yaw;

	const float NewYaw =
		FMath::FixedTurn(
			CurrentYaw,
			TargetYaw,
			MaxAngularVelocity * DeltaTime);

	MovementDirection =
		FRotator(
			0.f,
			NewYaw,
			0.f
		).Vector();
}