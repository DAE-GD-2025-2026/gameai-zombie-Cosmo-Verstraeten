#include "BTTask_UseMedkit.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "SurvivorInventoryLogic.h"


UBTTask_UseMedkit::UBTTask_UseMedkit()
{
	NodeName = TEXT("Use Medkit");
}

EBTNodeResult::Type UBTTask_UseMedkit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	const int32 MedkitSlot = InventoryLogic->FindUsableMedkitSlot();


	if (MedkitSlot == INDEX_NONE)
	{
		return EBTNodeResult::Failed;
	}


	const bool bUsed = InventoryLogic->UseItemAndCleanup( MedkitSlot);
	
	if (!bUsed)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}