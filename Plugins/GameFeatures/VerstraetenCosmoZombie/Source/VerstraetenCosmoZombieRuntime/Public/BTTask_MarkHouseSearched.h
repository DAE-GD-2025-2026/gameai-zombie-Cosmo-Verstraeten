#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

#include "BTTask_MarkHouseSearched.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_MarkHouseSearched
	: public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MarkHouseSearched();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetHouseKey;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AtTargetHouseKey;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetItemKey;
};
