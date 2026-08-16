#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

#include "BTTask_ShootZombie.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTTask_ShootZombie
	: public UBTTaskNode
{
	GENERATED_BODY()


public:

	UBTTask_ShootZombie();


protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetEnemyKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector WeaponSlotKey;
	
	UPROPERTY(EditAnywhere, Category = "Shooting")
	float MaxAngularVelocity{180.f};

	UPROPERTY(EditAnywhere, Category = "Shooting")
	float AimToleranceDegrees{3.f};

};
