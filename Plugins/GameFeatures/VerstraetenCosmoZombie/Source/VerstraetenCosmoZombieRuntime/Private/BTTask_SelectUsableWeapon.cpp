#include "BTTask_SelectUsableWeapon.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SurvivorInventoryLogic.h"



UBTTask_SelectUsableWeapon::UBTTask_SelectUsableWeapon()
{
	NodeName = TEXT("Select Usable Weapon");
	
	WeaponSlotKey.AddIntFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_SelectUsableWeapon,
			WeaponSlotKey));
}


EBTNodeResult::Type UBTTask_SelectUsableWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController =OwnerComp.GetAIOwner();
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


	USurvivorInventoryLogic* InventoryLogic = Pawn->FindComponentByClass<USurvivorInventoryLogic>();
	if (!InventoryLogic)
	{
		return EBTNodeResult::Failed;
	}
	
	const int32 WeaponSlot = InventoryLogic->FindUsableWeaponSlot();
	
	GEngine->AddOnScreenDebugMessage(
	-1,
	3.f,
	WeaponSlot == INDEX_NONE
		? FColor::Red
		: FColor::Green,
	FString::Printf(
		TEXT("Selected Weapon Slot = %d"),
		WeaponSlot));
	
	if (WeaponSlot == INDEX_NONE)
	{
		Blackboard->SetValueAsInt(WeaponSlotKey.SelectedKeyName,-1);
		return EBTNodeResult::Failed;
	}
	
	Blackboard->SetValueAsInt(WeaponSlotKey.SelectedKeyName, WeaponSlot);
	
	return EBTNodeResult::Succeeded;
}
