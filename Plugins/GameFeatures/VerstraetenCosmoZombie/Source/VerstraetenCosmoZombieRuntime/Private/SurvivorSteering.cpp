#include "SurvivorSteering.h"

#include "Survivor/SurvivorPawn.h"

#include "DrawDebugHelpers.h"
#include "StudentPerceptor.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Zombies/BaseZombie.h"

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
	bUseTargetLocation = false;
}

void USurvivorSteering::SetTargetLocation(const FVector& Location)
{
	TargetActor = nullptr;
	TargetLocation = Location;
	bUseTargetLocation = true;
}

FVector USurvivorSteering::GetTargetLocation() const
{
	if (bUseTargetLocation)
	{
		return TargetLocation;
	}

	if (TargetActor)
	{
		return TargetActor->GetActorLocation();
	}

	return FVector::ZeroVector;
}

void USurvivorSteering::SetSteeringWeights(const FSteeringWeights& Weights)
{
	CurrentWeights.Wander = FMath::Max(0.f, Weights.Wander);
	CurrentWeights.Seek = FMath::Max(0.f, Weights.Seek);
	CurrentWeights.Avoid = FMath::Max(0.f, Weights.Avoid);
}

void USurvivorSteering::StartSteering()
{
	if (CurrentWeights.Seek > 0.f)
	{
		if (TargetActor || bUseTargetLocation)
		{
			CreatePathToTarget();
		}
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
	
	BlendedPath.Empty();
	BlendedPathPointIndex = 0;
	BlendedRepathTimer = 0.f;

	SetComponentTickEnabled(false);
}


FVector USurvivorSteering::CalculateWanderDirection(float DeltaTime)
{
	if (!WanderPath.IsValidIndex(
		WanderPathPointIndex))
	{
		CreateNewWanderPath();
	}

	return FollowPath(WanderPath, WanderPathPointIndex, true);
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
	return FollowPath(SeekPath, SeekPathPointIndex, false);
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

	if (CurrentWeights.Seek > 0.f && CurrentWeights.Avoid > 0.f && TargetActor)
	{
		FinalDirection = CalculateBlendedDirection(DeltaTime);
	}

	else if (CurrentWeights.Seek > 0.f)
	{
		FinalDirection = CalculateSeekDirection();
	}
	
	else if (CurrentWeights.Wander > 0.f)
	{
		FinalDirection = CalculateWanderDirection(DeltaTime);
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

	if (!CachedPawn)
	{
		return;
	}
	
	if (!TargetActor && !bUseTargetLocation)
	{
		return;
	}

	const FVector Destination = GetTargetLocation();

	SeekPath = CachedPawn->CalculatePath(Destination);

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

FVector USurvivorSteering::FollowPath(TArray<FVector>& Path, int32& PathPointIndex, bool bFinishAtLastPoint)
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

	const bool bIsLastPathPoint = PathPointIndex == Path.Num() - 1;
	if (Distance <= PathPointAcceptanceRadius)
	{
		if (!bIsLastPathPoint)
		{
			++PathPointIndex;
		}

		else if (bFinishAtLastPoint)
		{
			++PathPointIndex;
		}
	}


	if (!Path.IsValidIndex(PathPointIndex))
	{
		return FVector::ZeroVector;
	}
	
	FVector Direction =Path[PathPointIndex] - PawnLocation;
	Direction.Z = 0.f;
	return Direction.GetSafeNormal();
}

FVector USurvivorSteering::CalculateAvoidDirection() const
{
	if (!CachedPawn)
	{
		return FVector::ZeroVector;
	}
	
	UStudentPerceptor* Perceptor = CachedPawn->FindComponentByClass<UStudentPerceptor>();
	if (!Perceptor)
	{
		return FVector::ZeroVector;
	}
	
	const FVector PawnLocation = CachedPawn->GetActorLocation();
	
	FVector AvoidDirection = FVector::ZeroVector;
	
	const TArray<TObjectPtr<ABaseZombie>>& Zombies = Perceptor->GetVisibleEnemies();
	
	for (const TObjectPtr<ABaseZombie>& ZombiePtr : Zombies)
	{
		ABaseZombie* Zombie = ZombiePtr.Get();

		if (!IsValid(Zombie))
		{
			continue;
		}
		
		FVector Away = PawnLocation - Zombie->GetActorLocation();
		Away.Z = 0.f;
		
		const float Distance = Away.Size();
		
		if (Distance <= KINDA_SMALL_NUMBER)
		{
			continue;
		}
		
		if (Distance > AvoidDistance)
		{
			continue;
		}
		
		Away.Normalize();
		// Closer enemy = stronger avoidance.
		const float Strength =
			1.f -
			FMath::Clamp(
				Distance / AvoidDistance,
				0.f,
				1.f);


		AvoidDirection += Away * Strength;
	}

	return AvoidDirection.GetSafeNormal();
}


FVector USurvivorSteering::CalculateBlendedIntent() const
{
	if (!CachedPawn || !TargetActor)
	{
		return FVector::ZeroVector;
	}


	FVector SeekDirection = TargetActor->GetTargetLocation() - CachedPawn->GetActorLocation();

	SeekDirection.Z = 0.f;
	SeekDirection.Normalize();

	const FVector AvoidDirection = CalculateAvoidDirection();

	FVector DesiredDirection = SeekDirection * CurrentWeights.Seek + AvoidDirection * CurrentWeights.Avoid;
	
	return DesiredDirection.GetSafeNormal();
}

void USurvivorSteering::CreateBlendedPath()
{
	if (!CachedPawn)
	{
		return;
	}

	const FVector DesiredDirection = CalculateBlendedIntent();
	if (DesiredDirection.IsNearlyZero())
	{
		return;
	}


	const FVector TemporaryTarget = CachedPawn->GetActorLocation() + DesiredDirection * BlendedMoveDistance;
	
	BlendedPath = CachedPawn->CalculatePath(TemporaryTarget);
	BlendedPathPointIndex = 0;
	if (BlendedPath.Num() > 1)
	{
		BlendedPathPointIndex = 1;
	}
}

FVector USurvivorSteering::CalculateBlendedDirection(float DeltaTime)
{
	BlendedRepathTimer += DeltaTime;


	if (BlendedRepathTimer >= BlendedRepathInterval || !BlendedPath.IsValidIndex(BlendedPathPointIndex))
	{
		BlendedRepathTimer = 0.f;
		CreateBlendedPath();
	}

	return FollowPath(BlendedPath, BlendedPathPointIndex,true);
}

