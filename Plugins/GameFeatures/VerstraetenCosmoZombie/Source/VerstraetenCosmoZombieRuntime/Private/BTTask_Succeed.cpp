#include "BTTask_Succeed.h"


UBTTask_Succeed::UBTTask_Succeed()
{
	NodeName = TEXT("Succeed");
}


EBTNodeResult::Type UBTTask_Succeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}
