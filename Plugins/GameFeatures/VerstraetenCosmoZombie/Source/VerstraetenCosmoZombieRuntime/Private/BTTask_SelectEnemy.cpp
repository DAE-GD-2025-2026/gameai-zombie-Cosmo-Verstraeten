#include "BTTask_SelectEnemy.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

#include "StudentPerceptor.h"

#include "Zombies/BaseZombie.h"


UBTTask_SelectEnemy::UBTTask_SelectEnemy()
{
	NodeName = TEXT("Select Enemy");
	
	TargetEnemyKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_SelectEnemy,
			TargetEnemyKey),
		AActor::StaticClass());
}


EBTNodeResult::Type UBTTask_SelectEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* Pawn =AIController->GetPawn();

	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* Blackboard =OwnerComp.GetBlackboardComponent();

	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	UStudentPerceptor* Perceptor =  Pawn->FindComponentByClass<UStudentPerceptor>();

	if (!Perceptor)
	{
		return EBTNodeResult::Failed;
	}
	
	const TArray<TObjectPtr<ABaseZombie>>&VisibleZombies = Perceptor->GetVisibleEnemies();
	ABaseZombie* ClosestEnemy = nullptr;
	float ClosestDistanceSquared = FLT_MAX;

	for (const TObjectPtr<ABaseZombie>& Zombie : VisibleZombies)
	{
		if (!IsValid(Zombie))
		{
			continue;
		}


		const float DistanceSquared = FVector::DistSquared2D(Pawn->GetActorLocation(), Zombie->GetActorLocation());

		if (DistanceSquared < ClosestDistanceSquared)
		{
			ClosestDistanceSquared = DistanceSquared;
			ClosestEnemy = Zombie.Get();
		}
	}
	
	if (!ClosestEnemy)
	{
		Blackboard->ClearValue(TargetEnemyKey.SelectedKeyName);
		return EBTNodeResult::Failed;
	}
	
	Blackboard->SetValueAsObject(TargetEnemyKey.SelectedKeyName, ClosestEnemy);


	return EBTNodeResult::Succeeded;
}
