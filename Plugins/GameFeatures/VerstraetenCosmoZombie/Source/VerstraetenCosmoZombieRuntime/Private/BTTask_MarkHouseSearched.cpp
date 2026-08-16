#include "BTTask_MarkHouseSearched.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

#include "StudentPerceptor.h"


UBTTask_MarkHouseSearched::UBTTask_MarkHouseSearched()
{
	NodeName = TEXT("Mark House Searched");


	TargetHouseKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_MarkHouseSearched,
			TargetHouseKey),
		AActor::StaticClass());


	AtTargetHouseKey.AddBoolFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_MarkHouseSearched,
			AtTargetHouseKey));


	TargetItemKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_MarkHouseSearched,
			TargetItemKey),
		AActor::StaticClass());
}


EBTNodeResult::Type UBTTask_MarkHouseSearched::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
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


	AActor* House =Cast<AActor>(Blackboard->GetValueAsObject(TargetHouseKey.SelectedKeyName));


	if (!House)
	{
		return EBTNodeResult::Failed;
	}
	
	Perceptor->MarkHouseSearched(House);

	Blackboard->SetValueAsBool(
	TEXT("HasKnownHouse"),
	Perceptor->HasUnsearchedHouse());

	Blackboard->ClearValue(TargetHouseKey.SelectedKeyName);

	Blackboard->SetValueAsBool(AtTargetHouseKey.SelectedKeyName,false);

	Blackboard->ClearValue(TargetItemKey.SelectedKeyName);
	
	return EBTNodeResult::Succeeded;
}
