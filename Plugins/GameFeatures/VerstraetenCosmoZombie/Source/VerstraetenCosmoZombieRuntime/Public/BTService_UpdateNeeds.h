#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTService_UpdateNeeds.generated.h"


UCLASS()
class VERSTRAETENCOSMOZOMBIERUNTIME_API UBTService_UpdateNeeds : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateNeeds();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedsMedkitKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NeedsFoodKey;
};