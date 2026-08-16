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
	float MaxWanderAngularVelocity{ 60.f };
	
	//seek
	TArray<FVector> CurrentPath{};
	int32 CurrentPathPointIndex{ 0 };
	float PathPointAcceptanceRadius{ 5.f };
};
