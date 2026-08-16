#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_Flee.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_Flee : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UBTTask_Flee();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
	void UpdateFleeDestination(UBehaviorTreeComponent& OwnerComp);


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetEnemyKey;

	UPROPERTY(EditAnywhere, Category = "Flee")
	float FleeDistance{800.f};
	
	UPROPERTY(EditAnywhere, Category = "Flee")
	float RepathInterval{0.5f};
	
	FVector FleeDestination{};
	float AcceptanceRadius{50.f};
};
