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

	USurvivorSteering* Steering = Pawn->FindComponentByClass<USurvivorSteering>();

	if (!Steering)
	{

		return EBTNodeResult::Failed;
	}

	
	FSteeringWeights Weights{};

	Weights.Wander = 1.f;
	Weights.Seek = 0.f;
	
	Steering->SetTargetActor(nullptr);

	Steering->SetSteeringWeights(Weights);

	Steering->StartSteering();

	
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		TEXT("STARTING WANDER")
	);

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

	USurvivorSteering* Steering = Pawn->FindComponentByClass<USurvivorSteering>();

	if (Steering)
	{
		Steering->StopSteering();
	}

	return EBTNodeResult::Aborted;
}
