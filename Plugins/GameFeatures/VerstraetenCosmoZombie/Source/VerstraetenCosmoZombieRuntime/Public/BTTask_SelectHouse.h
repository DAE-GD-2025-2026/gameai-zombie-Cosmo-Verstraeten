#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_SelectHouse.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_SelectHouse
	: public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectHouse();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetHouseKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AtTargetHouseKey;
};
