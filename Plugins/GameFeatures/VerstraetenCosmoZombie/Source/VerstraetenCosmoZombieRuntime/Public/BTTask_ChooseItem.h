#pragma once


#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_ChooseItem.generated.h"

class ABaseItem;

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
	
	UPROPERTY(EditAnywhere, Category = "Utility")
	float DistancePenalty{0.05f};

	UPROPERTY(EditAnywhere, Category = "Utility")
	float GarbageCleanupScore{-1000.f};
	
	
private:
	
	float CalculateItemScore(APawn* Pawn, ABaseItem* Item) const;
	
};
