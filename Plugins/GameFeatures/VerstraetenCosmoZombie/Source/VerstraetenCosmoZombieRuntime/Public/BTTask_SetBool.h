#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_SetBool.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_SetBool
	: public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetBool();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BoolKey;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool Value{ true };
};
