#include "BTTask_SetBool.h"

#include "BehaviorTree/BlackboardComponent.h"


UBTTask_SetBool::UBTTask_SetBool()
{
	NodeName = TEXT("Set Blackboard Bool");

	BoolKey.AddBoolFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_SetBool,
			BoolKey));
}


EBTNodeResult::Type UBTTask_SetBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();


	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}


	Blackboard->SetValueAsBool(BoolKey.SelectedKeyName, Value);


	return EBTNodeResult::Succeeded;
}