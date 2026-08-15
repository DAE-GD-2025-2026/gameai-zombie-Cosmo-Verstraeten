#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Wander.generated.h"

UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_Wander : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Wander();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
