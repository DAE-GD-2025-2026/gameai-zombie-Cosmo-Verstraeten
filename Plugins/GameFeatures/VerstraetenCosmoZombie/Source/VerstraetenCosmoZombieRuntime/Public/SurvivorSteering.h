#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorSteering.generated.h"

class ASurvivorPawn;
class AActor;
class UStudentPerceptor;



USTRUCT(BlueprintType)
struct FSteeringWeights
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Wander{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Seek{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Avoid{0.f};
};

struct FAvoidZone
{
	TWeakObjectPtr<AActor> Actor{};

	float Radius{0.f};
};
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VERSTRAETENCOSMOZOMBIERUNTIME_API USurvivorSteering : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivorSteering();
	
	void SetTargetActor(AActor* Target);
	void SetTargetLocation(const FVector& Location);
	FVector GetTargetLocation() const;
	void SetSteeringWeights(const FSteeringWeights& Weights);
	void CreatePathToTarget();

	void StartSteering();
	void StopSteering();

	FVector GetMovementDirection() const
	{
		return MovementDirection;
	}

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

private:
	FVector CalculateWanderDirection(float DeltaTime);
	FVector CalculateSeekDirection();
	void CreateNewWanderPath();
	FVector CalculateAvoidDirection() const;
	FVector CalculateBlendedIntent() const;
	void CalculateAngularVelocity(const FVector& DesiredDirection);
	void CreateBlendedPath();
	FVector CalculateBlendedDirection(float DeltaTime);
	FVector FollowPath(TArray<FVector>& Path, int32& PathPointIndex, bool bFinishAtLastPoint);
	
	
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
	float PathPointAcceptanceRadius{ 50.f };
	
	float AngularVelocity{ 0.f };

	float MaxAngularVelocity{ 360.f };
	
	
	TArray<FVector> SeekPath{};
	int32 SeekPathPointIndex{ 0 };

	TArray<FVector> WanderPath{};
	int32 WanderPathPointIndex{ 0 };
	
	TArray<FVector> BlendedPath{};
	int32 BlendedPathPointIndex{0};
	float BlendedRepathTimer{0.f};
	
	UPROPERTY(EditAnywhere, Category = "Steering")
	float BlendedRepathInterval{0.2f};
	UPROPERTY(EditAnywhere, Category = "Steering")
	float BlendedMoveDistance{400.f};
	UPROPERTY(EditAnywhere, Category = "Steering|Avoid")
	float AvoidDistance{800.f};

	FVector TargetLocation{};
	bool bUseTargetLocation{false};
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawPath{true};
	
};
