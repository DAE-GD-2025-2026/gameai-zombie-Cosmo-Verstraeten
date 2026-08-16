#include "BTTask_ChooseItem.h"


#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "StudentPerceptor.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"


UBTTask_ChooseItem::UBTTask_ChooseItem()
{
	NodeName = TEXT("Select Item");

	TargetItemKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_ChooseItem,
			TargetItemKey),
		AActor::StaticClass());
}


EBTNodeResult::Type UBTTask_ChooseItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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


	UStudentPerceptor* Perceptor = Pawn->FindComponentByClass<UStudentPerceptor>();

	if (!Perceptor)
	{
		return EBTNodeResult::Failed;
	}


	UInventoryComponent* Inventory = Pawn->FindComponentByClass<UInventoryComponent>();

	if (!Inventory)
	{
		return EBTNodeResult::Failed;
	}
	
	const TArray<ABaseItem*>& InventoryItems = Inventory->GetInventory();

	bool HasFreeSlot = false;

	for (ABaseItem* InventoryItem : InventoryItems)
	{
		if (!InventoryItem)
		{
			HasFreeSlot = true;
			break;
		}
	}

	if (!HasFreeSlot)
	{
		Blackboard->ClearValue(TargetItemKey.SelectedKeyName);

		return EBTNodeResult::Succeeded;
	}
	
	ABaseItem* ClosestItem = nullptr;
	float ClosestDistanceSquared = FLT_MAX;

	const TArray<TObjectPtr<ABaseItem>>& KnownItems = Perceptor->GetKnownItems();

	for (const TObjectPtr<ABaseItem>& KnownItem : KnownItems)
	{
		if (!KnownItem)
		{
			continue;
		}

		if (InventoryItems.Contains(KnownItem.Get()))
		{
			continue;
		}


		const float DistanceSquared =FVector::DistSquared(Pawn->GetActorLocation(), KnownItem->GetActorLocation());

		if (DistanceSquared < ClosestDistanceSquared)
		{
			ClosestDistanceSquared = DistanceSquared;

			ClosestItem = KnownItem.Get();
		}
	}
	
	if (!ClosestItem)
	{
		Blackboard->ClearValue( TargetItemKey.SelectedKeyName);

		return EBTNodeResult::Succeeded;
	}


	Blackboard->SetValueAsObject(TargetItemKey.SelectedKeyName,ClosestItem);


	return EBTNodeResult::Succeeded;
}