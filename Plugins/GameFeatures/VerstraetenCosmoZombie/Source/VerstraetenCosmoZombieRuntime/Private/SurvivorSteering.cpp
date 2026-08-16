#include "SurvivorSteering.h"

#include "Survivor/SurvivorPawn.h"

#include "DrawDebugHelpers.h"

#include "Camera/PlayerCameraManager.h"

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
	CurrentPath.Empty();
	CurrentPathPointIndex = 0;

	SetComponentTickEnabled(false);
}


FVector USurvivorSteering::CalculateWanderDirection(float DeltaTime)
{
	if (!CachedPawn)
	{
		return FVector::ZeroVector;
	}
	
	WanderChangeTimer -= DeltaTime;
	if (WanderChangeTimer <= 0.f)
	{
		WanderChangeTimer = WanderChangeInterval;

		WanderAngularVelocity = FMath::FRandRange(-MaxWanderAngularVelocity, MaxWanderAngularVelocity);
	}


	WanderAngle += FMath::DegreesToRadians(WanderAngularVelocity) * DeltaTime;


	const FVector CircleCenter = CachedPawn->GetActorLocation() + MovementDirection * WanderDistance;

	const FVector CircleOffset
	{
		FMath::Cos(WanderAngle) * WanderRadius,
		FMath::Sin(WanderAngle) * WanderRadius,
		0.f
	};


	const FVector WanderTarget = CircleCenter + CircleOffset;


	FVector WanderDirection = WanderTarget - CachedPawn->GetActorLocation();
	WanderDirection.Z = 0.f;

	return WanderDirection.GetSafeNormal();
}

FVector USurvivorSteering::CalculateSeekDirection()
{
	if (!CachedPawn)
	{
		return FVector::ZeroVector;
	}

	if (!CurrentPath.IsValidIndex(CurrentPathPointIndex))
	{
		return FVector::ZeroVector;
	}
	
	const FVector CurrentPathPoint = CurrentPath[CurrentPathPointIndex];

	const float DistanceToPathPoint = FVector::Dist2D(CachedPawn->GetActorLocation(), CurrentPathPoint);


	if (DistanceToPathPoint <= PathPointAcceptanceRadius)
	{
		++CurrentPathPointIndex;
	}


	if (!CurrentPath.IsValidIndex(CurrentPathPointIndex))
	{
		return FVector::ZeroVector;
	}


	const FVector TargetPathPoint = CurrentPath[CurrentPathPointIndex];


	FVector SeekDirection = TargetPathPoint - CachedPawn->GetActorLocation();

	SeekDirection.Z = 0.f;
	
	return SeekDirection.GetSafeNormal();
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
	
	if (!FinalDirection.IsNearlyZero())
	{
		FinalDirection.Normalize();

		MovementDirection = FinalDirection;
		
		FRotator NewRotation = CachedPawn->GetActorRotation();
		NewRotation.Yaw = MovementDirection.Rotation().Yaw;

		CachedPawn->AddMovementInput(MovementDirection,1.f);
		CachedPawn->SetActorRotation(NewRotation);
	}
}

void USurvivorSteering::CreatePathToTarget()
{
	CurrentPath.Empty();
	CurrentPathPointIndex = 0;

	if (!CachedPawn || !TargetActor)
	{
		return;
	}
	CurrentPath = CachedPawn->CalculatePath(TargetActor->GetActorLocation());

	if (CurrentPath.Num() > 1)
	{
		CurrentPathPointIndex = 1;
	}
}


