#include "BTTask_Wander.h"

#include "AIController.h"
#include "SurvivorSteering.h"

UBTTask_Wander::UBTTask_Wander()
{
	NodeName = TEXT("Wander");
}

EBTNodeResult::Type UBTTask_Wander::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	USurvivorSteering* SteeringComponent = Pawn->FindComponentByClass<USurvivorSteering>();

	if (!SteeringComponent)
	{

		return EBTNodeResult::Failed;
	}

	
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		TEXT("STARTING WANDER")
	);
	
	SteeringComponent->StartWander();

	return EBTNodeResult::InProgress;
}


EBTNodeResult::Type UBTTask_Wander::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return EBTNodeResult::Aborted;
	}

	APawn* Pawn = AIController->GetPawn();

	if (!Pawn)
	{
		return EBTNodeResult::Aborted;
	}

	USurvivorSteering* SteeringComponent = Pawn->FindComponentByClass<USurvivorSteering>();

	if (SteeringComponent)
	{
		SteeringComponent->StopSteering();
	}

	return EBTNodeResult::Aborted;
}
