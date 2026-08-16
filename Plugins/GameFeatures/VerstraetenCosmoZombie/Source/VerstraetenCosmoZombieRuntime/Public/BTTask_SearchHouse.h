#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SearchHouse.generated.h"

UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_SearchHouse : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SearchHouse();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	float RotationSpeed{ 360.f };

	float DegreesRotated{ 0.f };
};
