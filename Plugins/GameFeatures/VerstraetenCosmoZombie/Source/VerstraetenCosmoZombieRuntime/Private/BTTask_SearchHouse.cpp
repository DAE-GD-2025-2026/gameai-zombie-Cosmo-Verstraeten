#include "BTTask_SearchHouse.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SearchHouse::UBTTask_SearchHouse()
{
	NodeName = TEXT("Search House");

	bNotifyTick = true;
	bCreateNodeInstance = true;
}


EBTNodeResult::Type UBTTask_SearchHouse::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController =
		OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn =
		AIController->GetPawn();

	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	DegreesRotated = 0.f;

	return EBTNodeResult::InProgress;
}


void UBTTask_SearchHouse::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		return;
	}

	APawn* Pawn =
		AIController->GetPawn();

	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		return;
	}


	const float RotationThisFrame = RotationSpeed * DeltaSeconds;


	Pawn->AddActorWorldRotation(
		FRotator(0.f,RotationThisFrame,0.f)
	);

	DegreesRotated +=FMath::Abs(RotationThisFrame);


	if (DegreesRotated >= 360.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_SearchHouse::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	DegreesRotated = 0.f;

	return EBTNodeResult::Aborted;
}