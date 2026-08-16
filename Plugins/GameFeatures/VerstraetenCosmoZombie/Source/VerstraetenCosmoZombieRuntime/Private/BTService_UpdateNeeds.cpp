#include "BTService_UpdateNeeds.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SurvivorInventoryLogic.h"


UBTService_UpdateNeeds::UBTService_UpdateNeeds()
{
	NodeName = TEXT("Update Survivor Needs");
	bNotifyTick = true;

	Interval = 0.25f;
	RandomDeviation = 0.f;
	
	NeedsMedkitKey.AddBoolFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTService_UpdateNeeds,
			NeedsMedkitKey));


	NeedsFoodKey.AddBoolFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTService_UpdateNeeds,
			NeedsFoodKey));
}

void UBTService_UpdateNeeds::TickNode(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(
		OwnerComp,
		NodeMemory,
		DeltaSeconds);
	
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
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Blackboard)
	{
		return;
	}
	
	USurvivorInventoryLogic* InventoryLogic = Pawn->FindComponentByClass<USurvivorInventoryLogic>();

	if (!InventoryLogic)
	{
		return;
	}
	const bool bNeedsMedkit = InventoryLogic->ShouldUseMedkit() && InventoryLogic->FindUsableMedkitSlot() != INDEX_NONE;
	const bool bNeedsFood =InventoryLogic->ShouldUseFood() && InventoryLogic->FindUsableFoodSlot() != INDEX_NONE;
	
	Blackboard->SetValueAsBool(NeedsMedkitKey.SelectedKeyName, bNeedsMedkit);
	
	Blackboard->SetValueAsBool(NeedsFoodKey.SelectedKeyName, bNeedsFood);
}