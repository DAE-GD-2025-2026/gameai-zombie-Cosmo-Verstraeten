#include "BTTask_ChooseItem.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "StudentPerceptor.h"
#include "SurvivorInventoryLogic.h"

#include "Items/BaseItem.h"

#include <cfloat>


UBTTask_ChooseItem::UBTTask_ChooseItem()
{
	NodeName = TEXT("Choose Item");


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


	USurvivorInventoryLogic* InventoryLogic = Pawn->FindComponentByClass<USurvivorInventoryLogic>();

	if (!InventoryLogic)
	{
		return EBTNodeResult::Failed;
	}
	
	const TArray<TObjectPtr<ABaseItem>>& KnownItems = Perceptor->GetKnownItems();
	
	ABaseItem* BestItem = nullptr;
	float BestScore = -FLT_MAX;
	
	for (const TObjectPtr<ABaseItem>& ItemPtr : KnownItems)
	{
		ABaseItem* Item = ItemPtr.Get();

		if (!IsValid(Item))
		{
			continue;
		}

		if (!InventoryLogic->CanTakeItem(Item))
		{
			continue;
		}
		
		float Score = 0.f;


		if (InventoryLogic->IsGarbage(Item))
		{
			Score = GarbageCleanupScore;
		}
		else
		{
			Score = InventoryLogic->CalculateItemUtility(Item,false);
		}
		
		const float Distance = FVector::Dist2D(Pawn->GetActorLocation(), Item->GetActorLocation());
		
		Score -= Distance * DistancePenalty;
		
		if (Score > BestScore)
		{
			BestScore = Score;
			BestItem = Item;
		}
	}
	
	if (!BestItem)
	{
		Blackboard->ClearValue(TargetItemKey.SelectedKeyName);
		return EBTNodeResult::Succeeded;
	}


	Blackboard->SetValueAsObject(TargetItemKey.SelectedKeyName, BestItem);


	return EBTNodeResult::Succeeded;
}