#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_ChooseItem.generated.h"

UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_ChooseItem : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_ChooseItem();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetItemKey;
};
