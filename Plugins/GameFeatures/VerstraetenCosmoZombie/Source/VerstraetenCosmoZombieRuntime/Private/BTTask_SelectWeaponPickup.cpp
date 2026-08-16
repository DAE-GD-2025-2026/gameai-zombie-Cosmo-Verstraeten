#include "BTTask_SelectWeaponPickup.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "StudentPerceptor.h"
#include "SurvivorInventoryLogic.h"

#include "Items/BaseItem.h"
#include "Items/Weapon.h"


UBTTask_SelectWeaponPickup::UBTTask_SelectWeaponPickup()
{
	NodeName =
		TEXT("Select Weapon Pickup");

	TargetWeaponPickupKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_SelectWeaponPickup,
			TargetWeaponPickupKey),
		AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_SelectWeaponPickup::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	
	UStudentPerceptor* Perceptor = Pawn->FindComponentByClass<UStudentPerceptor>();
	if (!Perceptor)
	{
		return EBTNodeResult::Failed;
	}

	USurvivorInventoryLogic* InventoryLogic = Pawn->FindComponentByClass<USurvivorInventoryLogic>();
	if (!InventoryLogic)
	{
		return EBTNodeResult::Failed;
	}
	
	AWeapon* BestWeapon = nullptr;
	float BestScore = -FLT_MAX;

	const TArray<TObjectPtr<ABaseItem>>& KnownItems = Perceptor->GetKnownItems();
	
	for (const TObjectPtr<ABaseItem>& ItemPtr : KnownItems)
	{
		ABaseItem* Item = ItemPtr.Get();
		if (!IsValid(Item))
		{
			continue;
		}
		
		AWeapon* Weapon = Cast<AWeapon>(Item);
		if (!Weapon)
		{
			continue;
		}
		
		if (Weapon->GetValue() <= 0)
		{
			continue;
		}
		
		if (!InventoryLogic->CanTakeItem(Weapon))
		{
			continue;
		}

		float Score = InventoryLogic->CalculateItemUtility(Weapon,false);
		
		const float Distance =FVector::Dist2D(Pawn->GetActorLocation(), Weapon->GetActorLocation());
		Score -= Distance * 0.1f;

		if (Score > BestScore)
		{
			BestScore = Score;
			BestWeapon = Weapon;
		}
	}
	
	if (!BestWeapon)
	{
		Blackboard->ClearValue(TargetWeaponPickupKey.SelectedKeyName);
		Blackboard->SetValueAsBool(TEXT("HasKnownWeaponPickup"),false);
		return EBTNodeResult::Failed;
	}
	
	Blackboard->SetValueAsObject(TargetWeaponPickupKey.SelectedKeyName, BestWeapon);

	return EBTNodeResult::Succeeded;
}
