#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

#include "BTTask_SelectWeaponPickup.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_SelectWeaponPickup : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_SelectWeaponPickup();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetWeaponPickupKey;
};
