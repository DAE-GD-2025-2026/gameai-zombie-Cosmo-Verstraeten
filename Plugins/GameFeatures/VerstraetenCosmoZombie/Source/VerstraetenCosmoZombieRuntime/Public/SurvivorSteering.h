#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorSteering.generated.h"

class APawn;

UENUM()
enum class ESurvivorSteeringMode : uint8
{
	None,
	Wander,
	Seek,
	Flee
};
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class VERSTRAETENCOSMOZOMBIERUNTIME_API USurvivorSteering : public UActorComponent
{
public:GENERATED_BODY()

public:
	USurvivorSteering();

	void StartWander();
	void StopSteering();

protected:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void BeginPlay() override;

private:
	void UpdateWander(float DeltaTime);
	//void FaceDirection(const FVector& DesiredDirection, float DeltaTime);
	void RotateMoveDirection(const FVector& DesiredDirection, float DeltaTime);
	ESurvivorSteeringMode SteeringMode
	{
		ESurvivorSteeringMode::None
	};
	
	UPROPERTY()
	TObjectPtr<APawn> CachedPawn{ nullptr };


	float WanderDistance{ 40.f };

	float WanderRadius{ 20.f };

	float MaxAngleChangePerSecond{ 90.f };
	
	float WanderAngle{ 0.f };

	
	float MaxAngularVelocity{ 40.f };
	
	FVector MovementDirection{ FVector::ForwardVector };
};
