#include "BTTask_GrabItem.h"

#include "AIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"
#include "StudentPerceptor.h"

#include "Items/BaseItem.h"

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


EBTNodeResult::Type UBTTask_GrabItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
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


	UObject* TargetObject = Blackboard->GetValueAsObject(TargetItemKey.SelectedKeyName);

	ABaseItem* Item = Cast<ABaseItem>(TargetObject);

	if (!Item)
	{
		return EBTNodeResult::Failed;
	}


	UInventoryComponent* Inventory = Pawn->FindComponentByClass<UInventoryComponent>();

	if (!Inventory)
	{
		return EBTNodeResult::Failed;
	}
	
	const float Distance =
	FVector::Dist(Pawn->GetActorLocation(), Item->GetActorLocation());


	if (Distance > Inventory->GetPickupRange())
	{
		return EBTNodeResult::Failed;
	}
	
	const TArray<ABaseItem*>& InventoryItems = Inventory->GetInventory();

	int32 FreeSlot = INDEX_NONE;

	for (int32 Index = 0; Index < InventoryItems.Num(); ++Index)
	{
		if (!InventoryItems[Index])
		{
			FreeSlot = Index;
			break;
		}
	}
	
	if (FreeSlot == INDEX_NONE)
	{
		Blackboard->ClearValue(TargetItemKey.SelectedKeyName);

		return EBTNodeResult::Failed;
	}
	
	const bool Grabbed = Inventory->GrabItem(FreeSlot, Item);

	if (!Grabbed)
	{
		return EBTNodeResult::Failed;
	}
	
	UStudentPerceptor* Perceptor = Pawn->FindComponentByClass<UStudentPerceptor>();

	if (Perceptor)
	{
		Perceptor->ForgetItem(Item);
	}
	
	Blackboard->ClearValue(TargetItemKey.SelectedKeyName);


	return EBTNodeResult::Succeeded;
}