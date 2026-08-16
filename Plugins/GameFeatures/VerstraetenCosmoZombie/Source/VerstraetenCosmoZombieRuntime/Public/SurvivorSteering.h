#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorSteering.generated.h"

class ASurvivorPawn;
class AActor;


USTRUCT(BlueprintType)
struct FSteeringWeights
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Wander{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Seek{ 0.f };

	//float Flee{ 0.f };

	//float Pursuit{ 0.f };
};
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VERSTRAETENCOSMOZOMBIERUNTIME_API USurvivorSteering : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorSteering();
	
	void SetTargetActor(AActor* Target);
	//void SetThreatActor(AActor* Threat);

	void SetSteeringWeights(const FSteeringWeights& Weights);

	void StartSteering();
	void StopSteering();
	
	FVector GetMovementDirection() const
	{
		return MovementDirection;
	}

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	

private:
	void CreatePathToTarget();
	
	FVector CalculateWanderDirection(float DeltaTime);

	FVector CalculateSeekDirection();
	void CreateNewWanderPath();
	void CalculateAngularVelocity(const FVector& DesiredDirection);
	
	FVector FollowPath(
	TArray<FVector>& Path,
	int32& PathPointIndex);
	
	
	UPROPERTY()
	TObjectPtr<ASurvivorPawn> CachedPawn{ nullptr };
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor{ nullptr };
	FSteeringWeights CurrentWeights{};
	FVector MovementDirection{ FVector::ForwardVector };
	
	

	//wander 
	float WanderDistance{ 150.f };
	float WanderRadius{ 50.f };
	float WanderAngle{ 0.f };
	
	float WanderAngularVelocity{ 0.f };
	float WanderChangeTimer{ 0.f };
	float WanderChangeInterval{ 0.4f };
	float MaxWanderAngleChange{ 90.f };
	
	float WanderGoalChangeInterval{ 1.5f };

	float WanderGoalTimer{ 0.f };

	FVector CurrentWanderGoal{};

	
	//seek
	TArray<FVector> CurrentPath{};
	int32 CurrentPathPointIndex{ 0 };
	float PathPointAcceptanceRadius{ 5.f };
	
	float AngularVelocity{ 0.f };

	float MaxAngularVelocity{ 360.f };
	
	
	TArray<FVector> SeekPath{};
	int32 SeekPathPointIndex{ 0 };

	TArray<FVector> WanderPath{};
	int32 WanderPathPointIndex{ 0 };
	
};
