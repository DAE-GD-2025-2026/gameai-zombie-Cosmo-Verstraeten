#include "BTTask_Seek.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"


UBTTask_Seek::UBTTask_Seek()
{
	NodeName = TEXT("Seek");

	bNotifyTick = true;


	TargetKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_Seek,
			TargetKey),
		AActor::StaticClass());
}


EBTNodeResult::Type UBTTask_Seek::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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


	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}


	USurvivorSteering* Steering = Pawn->FindComponentByClass<USurvivorSteering>();

	if (!Steering)
	{
		return EBTNodeResult::Failed;
	}


	Steering->SetTargetActor(TargetActor);

	Steering->SetSteeringWeights(SteeringWeights);

	Steering->StartSteering();
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		TEXT("STARTING SEEK")
	);

	return EBTNodeResult::InProgress;
}

void UBTTask_Seek::TickTask(UBehaviorTreeComponent& OwnerComp,  uint8* NodeMemory, float DeltaSeconds)
{
	GEngine->AddOnScreenDebugMessage(
	-1,
	0.f,
	FColor::Yellow,
	TEXT("SEEK TICK"));
	
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		UE_LOG(
	LogTemp,
	Error,
	TEXT("SEEK FINISHED: NO AI CONTROLLER"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		return;
	}


	APawn* Pawn = AIController->GetPawn();

	if (!Pawn)
	{
		
		UE_LOG(
	LogTemp,
	Error,
	TEXT("SEEK FINISHED: NO Pawnnn CONTROLLER"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}


	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Blackboard)
	{
		UE_LOG(
	LogTemp,
	Error,
	TEXT("SEEK FINISHED: NO blackbo CONTROLLER"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}


	AActor* TargetActor =Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!TargetActor)
	{
		UE_LOG(
	LogTemp,
	Error,
	TEXT("SEEK FINISHED: NO targetact CONTROLLER"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}


	const float DistanceToTarget = FVector::Dist2D(Pawn->GetActorLocation(), TargetActor->GetActorLocation());


	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Seek distance: %f | Acceptance: %f"),
		DistanceToTarget,
		AcceptanceRadius);
	if (DistanceToTarget <= AcceptanceRadius)
	{
		UE_LOG(
	LogTemp,
	Error,
	TEXT("SEEK SUCCEEDED: INSIDE ACCEPTANCE RADIUS"));
		USurvivorSteering* Steering = Pawn->FindComponentByClass<USurvivorSteering>();

		if (Steering)
		{
			Steering->StopSteering();
		}


		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_Seek::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	
	UE_LOG(
		LogTemp,
		Error,
		TEXT("SEEK WAS ABORTED"));
	
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController)
	{
		APawn* Pawn = AIController->GetPawn();

		if (Pawn)
		{
			USurvivorSteering* Steering = Pawn->FindComponentByClass<USurvivorSteering>();

			if (Steering)
			{
				Steering->StopSteering();
			}
		}
	}
	return EBTNodeResult::Aborted;
}