#include "SurvivorSteering.h"

#include "Survivor/SurvivorPawn.h"

#include "DrawDebugHelpers.h"

#include "Camera/PlayerCameraManager.h"
#include "GameFramework/FloatingPawnMovement.h"

USurvivorSteering::USurvivorSteering()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USurvivorSteering::BeginPlay()
{
	Super::BeginPlay();

	CachedPawn = Cast<ASurvivorPawn>(GetOwner());

	if (!CachedPawn)
	{
		return;
	}
	
	MovementDirection = CachedPawn->GetActorForwardVector();
	MovementDirection.Z = 0.f;
	MovementDirection.Normalize();
	
	WanderAngle = FMath::DegreesToRadians(MovementDirection.Rotation().Yaw);
	
	
	UFloatingPawnMovement* PawnMovement = CachedPawn->FindComponentByClass<UFloatingPawnMovement>();

	if (PawnMovement)
	{
		PawnMovement->AddTickPrerequisiteComponent(this);
	}
	
}

void USurvivorSteering::SetTargetActor(AActor* Target)
{
	TargetActor = Target;
}

void USurvivorSteering::SetSteeringWeights(const FSteeringWeights& Weights)
{
	CurrentWeights.Wander = FMath::Max(0.f, Weights.Wander);

	CurrentWeights.Seek = FMath::Max(0.f, Weights.Seek);
}

void USurvivorSteering::StartSteering()
{
	if (CurrentWeights.Seek > 0.f && TargetActor)
	{
		CreatePathToTarget();
	}
	
	SetComponentTickEnabled(true);
}

void USurvivorSteering::StopSteering()
{
	CurrentWeights = FSteeringWeights{};

	TargetActor = nullptr;
	SeekPath.Empty();
	SeekPathPointIndex = 0;

	WanderPath.Empty();
	WanderPathPointIndex = 0;

	SetComponentTickEnabled(false);
}


FVector USurvivorSteering::CalculateWanderDirection(float DeltaTime)
{
	if (!WanderPath.IsValidIndex(
		WanderPathPointIndex))
	{
		CreateNewWanderPath();
	}

	return FollowPath(
		WanderPath,
		WanderPathPointIndex);
}

void USurvivorSteering::CreateNewWanderPath()
{
	if (!CachedPawn)
	{
		return;
	}


	const FVector CircleCenter = CachedPawn->GetActorLocation() + MovementDirection * WanderDistance;


	WanderAngle += FMath::DegreesToRadians(FMath::FRandRange(-MaxWanderAngleChange, MaxWanderAngleChange));

	const FVector CircleOffset
	{
		FMath::Cos(WanderAngle) * WanderRadius,
		FMath::Sin(WanderAngle) * WanderRadius,
		0.f
	};


	const FVector WanderTarget = CircleCenter + CircleOffset;


	WanderPath = CachedPawn->CalculatePath(WanderTarget);


	WanderPathPointIndex = 0;

	if (WanderPath.Num() > 1)
	{
		WanderPathPointIndex = 1;
	}
}

FVector USurvivorSteering::CalculateSeekDirection()
{
	return FollowPath(SeekPath, SeekPathPointIndex);
}

void USurvivorSteering::TickComponent(float DeltaTime,ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(
		DeltaTime,
		TickType,
		ThisTickFunction);

	if (!CachedPawn)
	{
		return;
	}
	
	FVector FinalDirection = FVector::ZeroVector;

	if (CurrentWeights.Wander > 0.f)
	{
		const FVector WanderDirection = CalculateWanderDirection(DeltaTime);

		FinalDirection += WanderDirection * CurrentWeights.Wander;
	}

	if (CurrentWeights.Seek > 0.f)
	{
		const FVector SeekDirection = CalculateSeekDirection();

		FinalDirection += SeekDirection * CurrentWeights.Seek;
	}
	
	if (FinalDirection.IsNearlyZero())
	{
		return;
	}
	
	FinalDirection.Normalize();

	MovementDirection = FinalDirection;
	CalculateAngularVelocity(MovementDirection);

	const float RotationThisFrame = AngularVelocity * DeltaTime;
	CachedPawn->AddActorWorldRotation(FRotator(0.f, RotationThisFrame,0.f));
	CachedPawn->AddMovementInput(MovementDirection,1.f);


	
}

void USurvivorSteering::CreatePathToTarget()
{
	SeekPath.Empty();
	SeekPathPointIndex = 0;

	if (!CachedPawn || !TargetActor)
	{
		return;
	}


	SeekPath  = CachedPawn->CalculatePath(TargetActor->GetActorLocation());


	if (SeekPath.Num() > 1)
	{
		SeekPathPointIndex = 1;
	}
}

void USurvivorSteering::CalculateAngularVelocity(const FVector& DesiredDirection)
{
	if (!CachedPawn || DesiredDirection.IsNearlyZero())
	{
		AngularVelocity = 0.f;
		return;
	}
	
	const float CurrentYaw = CachedPawn->GetActorRotation().Yaw;
	const float DesiredYaw = DesiredDirection.Rotation().Yaw;


	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw);
	
	
	if (FMath::Abs(DeltaYaw) < 1.f)
	{
		AngularVelocity = 0.f;
		return;
	}


	AngularVelocity = FMath::Sign(DeltaYaw) * MaxAngularVelocity;
}

FVector USurvivorSteering::FollowPath(
	TArray<FVector>& Path,
	int32& PathPointIndex)
{
	if (!CachedPawn)
	{
		return FVector::ZeroVector;
	}

	if (!Path.IsValidIndex(PathPointIndex))
	{
		return FVector::ZeroVector;
	}


	const FVector PawnLocation = CachedPawn->GetActorLocation();

	const FVector PathPoint =Path[PathPointIndex];


	const float Distance = FVector::Dist2D(PawnLocation, PathPoint);


	if (Distance <= PathPointAcceptanceRadius)
	{
		++PathPointIndex;
	}


	if (!Path.IsValidIndex(PathPointIndex))
	{
		return FVector::ZeroVector;
	}


	FVector Direction =Path[PathPointIndex] - PawnLocation;
	Direction.Z = 0.f;
	return Direction.GetSafeNormal();
}


