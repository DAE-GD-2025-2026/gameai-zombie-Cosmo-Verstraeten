#include "SurvivorSteering.h"

#include "GameFramework/Pawn.h"

USurvivorSteering::USurvivorSteering()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USurvivorSteering::StartWander()
{
	SteeringMode = ESurvivorSteeringMode::Wander;
	


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
	const float MaxAngleChange =
		FMath::DegreesToRadians(MaxAngleChangePerSecond) * DeltaTime;

	const float AngleChange = FMath::FRandRange(-MaxAngleChange,MaxAngleChange);

	WanderAngle += AngleChange;
	



	FVector Forward = MovementDirection;

	FVector Right = FVector::CrossProduct( FVector::UpVector,Forward);
	Right.Normalize();
	

	const FVector CircleCenter = CachedPawn->GetActorLocation() + Forward * WanderDistance;


	const FVector CircleOffset = Forward * FMath::Cos(WanderAngle) * WanderRadius  + Right * FMath::Sin(WanderAngle) * WanderRadius;


	const FVector WanderTarget =
		CircleCenter + CircleOffset;


	FVector DesiredDirection = WanderTarget - CachedPawn->GetActorLocation();

	DesiredDirection.Z = 0.f;
	DesiredDirection.Normalize();

	RotateMoveDirection(
		DesiredDirection,
		DeltaTime);

	CachedPawn->AddMovementInput(MovementDirection, 1.f);
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