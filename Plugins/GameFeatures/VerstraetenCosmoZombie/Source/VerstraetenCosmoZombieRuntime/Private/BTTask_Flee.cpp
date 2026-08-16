#include "BTTask_Flee.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "SurvivorSteering.h"
#include "Zombies/BaseZombie.h"

UBTTask_Flee::UBTTask_Flee()
{
	NodeName = TEXT("Flee");

	bNotifyTick = true;
	bCreateNodeInstance = true;


	TargetEnemyKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_Flee,
			TargetEnemyKey),
		AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller =OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}
	
	USurvivorSteering* Steering = Pawn->FindComponentByClass<USurvivorSteering>();
	if (!Steering)
	{
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* Blackboard =OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	
	ABaseZombie* Enemy = Cast<ABaseZombie>(Blackboard->GetValueAsObject(TargetEnemyKey.SelectedKeyName));

	if (!IsValid(Enemy))
	{
		return EBTNodeResult::Failed;
	}
	
	FVector AwayDirection =
		Pawn->GetActorLocation()
		-
		Enemy->GetActorLocation();

	AwayDirection.Z = 0.f;
	
	if (AwayDirection.IsNearlyZero())
	{
		return EBTNodeResult::Failed;
	}
	
	AwayDirection.Normalize();
	
	FleeDestination = Pawn->GetActorLocation() + AwayDirection * FleeDistance;
	
	Steering->SetTargetLocation(FleeDestination);
	
	FSteeringWeights Weights{};
	Weights.Seek = 1.f;
	Weights.Wander = 0.f;
	Weights.Avoid = 0.f;
	Steering->SetSteeringWeights(Weights);
	Steering->StartSteering();

	return EBTNodeResult::InProgress;
}

void UBTTask_Flee::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return;
	}
	
	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return;
	}
	

	
	const float Distance =
		FVector::Dist2D(Pawn->GetActorLocation(),FleeDestination);


	if (Distance <= AcceptanceRadius)
	{
		USurvivorSteering* Steering =Pawn->FindComponentByClass<USurvivorSteering>();
		if (Steering)
		{
			Steering->CreatePathToTarget();
		}
	}
}

EBTNodeResult::Type UBTTask_Flee::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();

	if (Controller)
	{
		APawn* Pawn = Controller->GetPawn();

		if (Pawn)
		{
			USurvivorSteering* Steering =Pawn->FindComponentByClass<USurvivorSteering>();
			if (Steering)
			{
				Steering->StopSteering();
			}
		}
	}
	return EBTNodeResult::Aborted;
}
