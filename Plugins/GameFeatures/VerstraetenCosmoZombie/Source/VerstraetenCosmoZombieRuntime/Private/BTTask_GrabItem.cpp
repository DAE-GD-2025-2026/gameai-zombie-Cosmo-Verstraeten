#include "BTTask_GrabItem.h"

#include "AIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"
#include "StudentPerceptor.h"

#include "Items/BaseItem.h"
#include "SurvivorInventoryLogic.h"

UBTTask_GrabItem::UBTTask_GrabItem()
{
	NodeName = TEXT("Grab Item");

	TargetItemKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_GrabItem,
			TargetItemKey),
		AActor::StaticClass());
}



EBTNodeResult::Type UBTTask_GrabItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* Pawn = AIController->GetPawn();

	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	
	ABaseItem* TargetItem = Cast<ABaseItem>(Blackboard->GetValueAsObject(TargetItemKey.SelectedKeyName));

	if (!IsValid(TargetItem))
	{
		Blackboard->ClearValue(TargetItemKey.SelectedKeyName);
		return EBTNodeResult::Failed;
	}


	USurvivorInventoryLogic* InventoryLogic = Pawn->FindComponentByClass<USurvivorInventoryLogic>();

	if (!InventoryLogic)
	{
		return EBTNodeResult::Failed;
	}


	UStudentPerceptor* Perceptor = Pawn->FindComponentByClass<UStudentPerceptor>();

	if (!Perceptor)
	{
		return EBTNodeResult::Failed;
	}
	
	const bool bGrabbed = InventoryLogic->TryGrabItem(TargetItem);


	if (!bGrabbed)
	{
		return EBTNodeResult::Failed;
	}

	Perceptor->ForgetItem(TargetItem);
	
	Blackboard->ClearValue(TargetItemKey.SelectedKeyName);


	return EBTNodeResult::Succeeded;
}