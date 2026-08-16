#include "BTTask_SelectHouse.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

#include "StudentPerceptor.h"


UBTTask_SelectHouse::UBTTask_SelectHouse()
{
	NodeName = TEXT("Select House");


	TargetHouseKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_SelectHouse,
			TargetHouseKey),
		AActor::StaticClass());


	AtTargetHouseKey.AddBoolFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_SelectHouse,
			AtTargetHouseKey));
}


EBTNodeResult::Type UBTTask_SelectHouse::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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


	UStudentPerceptor* Perceptor = Pawn->FindComponentByClass<UStudentPerceptor>();

	if (!Perceptor)
	{
		return EBTNodeResult::Failed;
	}

	AActor* CurrentTargetHouse =
		Cast<AActor>(Blackboard->GetValueAsObject( TargetHouseKey.SelectedKeyName));


	if (CurrentTargetHouse && !Perceptor->IsHouseSearched(CurrentTargetHouse))
	{
		return EBTNodeResult::Succeeded;
	}
	

	const TArray<TObjectPtr<AActor>>& KnownHouses = Perceptor->GetKnownHouses();


	AActor* BestHouse = nullptr;

	float BestDistanceSquared = FLT_MAX;


	for (const TObjectPtr<AActor>& KnownHouse : KnownHouses)
	{
		AActor* House = KnownHouse.Get();


		if (!IsValid(House))
		{
			continue;
		}


		if (Perceptor->IsHouseSearched(House))
		{
			continue;
		}


		const float DistanceSquared = FVector::DistSquared2D(Pawn->GetActorLocation(), House->GetActorLocation());


		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;

			BestHouse = House;
		}
	}

	if (!BestHouse)
	{
		Blackboard->ClearValue(TargetHouseKey.SelectedKeyName);

		Blackboard->SetValueAsBool(AtTargetHouseKey.SelectedKeyName,false);

		return EBTNodeResult::Failed;
	}
	
	
	Blackboard->SetValueAsObject(TargetHouseKey.SelectedKeyName, BestHouse);

	Blackboard->SetValueAsBool(AtTargetHouseKey.SelectedKeyName,false);


	return EBTNodeResult::Succeeded;
}
