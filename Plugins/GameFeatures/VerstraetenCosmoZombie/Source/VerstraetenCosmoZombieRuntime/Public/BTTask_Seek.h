#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

#include "SurvivorSteering.h"

#include "BTTask_Seek.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_Seek
	: public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Seek();


protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;


	UPROPERTY(EditAnywhere, Category = "Steering")
	FSteeringWeights SteeringWeights{};


	UPROPERTY(EditAnywhere, Category = "Steering")
	float AcceptanceRadius{ 100.f };
};
