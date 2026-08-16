#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_SelectUsableWeapon.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_SelectUsableWeapon : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectUsableWeapon();


protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector WeaponSlotKey;
};
