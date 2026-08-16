#include "BTTask_UseFood.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "SurvivorInventoryLogic.h"


UBTTask_UseFood::UBTTask_UseFood()
{
	NodeName = TEXT("Use Food");
}


EBTNodeResult::Type UBTTask_UseFood::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}
	
	USurvivorInventoryLogic* InventoryLogic = Pawn->FindComponentByClass<USurvivorInventoryLogic>();

	if (!InventoryLogic)
	{
		return EBTNodeResult::Failed;
	}

	const int32 FoodSlot = InventoryLogic->FindUsableFoodSlot();
	if (FoodSlot == INDEX_NONE)
	{
		return EBTNodeResult::Failed;
	}
	
	const bool bUsed = InventoryLogic->UseItemAndCleanup(FoodSlot);


	if (!bUsed)
	{
		return EBTNodeResult::Failed;
	}


	return EBTNodeResult::Succeeded;
}