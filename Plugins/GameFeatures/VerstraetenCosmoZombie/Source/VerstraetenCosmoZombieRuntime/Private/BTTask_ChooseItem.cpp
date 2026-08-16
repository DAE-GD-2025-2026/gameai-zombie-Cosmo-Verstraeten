#include "BTTask_ChooseItem.h"


#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "StudentPerceptor.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Items/Food.h"
#include "Items/Medkit.h"
#include "Items/Pistol.h"
#include "Items/Shotgun.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"


UBTTask_ChooseItem::UBTTask_ChooseItem()
{
	NodeName = TEXT("Select Item");

	TargetItemKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_ChooseItem,
			TargetItemKey),
		AActor::StaticClass());
}


EBTNodeResult::Type UBTTask_ChooseItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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


	UStudentPerceptor* Perceptor = Pawn->FindComponentByClass<UStudentPerceptor>();

	if (!Perceptor)
	{
		return EBTNodeResult::Failed;
	}


	UInventoryComponent* Inventory = Pawn->FindComponentByClass<UInventoryComponent>();

	if (!Inventory)
	{
		return EBTNodeResult::Failed;
	}
	
	const TArray<ABaseItem*>& InventoryItems = Inventory->GetInventory();

	bool HasFreeSlot = false;

	for (ABaseItem* InventoryItem : InventoryItems)
	{
		if (!InventoryItem)
		{
			HasFreeSlot = true;
			break;
		}
	}

	if (!HasFreeSlot)
	{
		Blackboard->ClearValue(TargetItemKey.SelectedKeyName);

		return EBTNodeResult::Succeeded;
	}
	
	ABaseItem* BestItem = nullptr;
	float ClosestDistanceSquared = FLT_MAX;

	const TArray<TObjectPtr<ABaseItem>>& KnownItems = Perceptor->GetKnownItems();
	
	float BestScore = 0.0f;

	for (const TObjectPtr<ABaseItem>& KnownItem : KnownItems)
	{
		if (!KnownItem)
		{
			continue;
		}

		if (InventoryItems.Contains(KnownItem.Get()))
		{
			continue;
		}


		const float Score = CalculateItemScore(Pawn, KnownItem.Get());

		if (Score > BestScore)
		{
			BestScore = Score;
			BestItem = KnownItem.Get();
		}
	}
	
	if (!BestItem)
	{
		Blackboard->ClearValue( TargetItemKey.SelectedKeyName);

		return EBTNodeResult::Succeeded;
	}


	Blackboard->SetValueAsObject(TargetItemKey.SelectedKeyName,BestItem);


	return EBTNodeResult::Succeeded;
}

float UBTTask_ChooseItem::CalculateItemScore(APawn* Pawn, ABaseItem* Item) const
{
	if (!Pawn || !Item)
	{
		return -FLT_MAX;
	}
	
	UInventoryComponent* Inventory = Pawn->FindComponentByClass<UInventoryComponent>();

	if (!Inventory)
	{
		return -FLT_MAX;
	}
	
	const TArray<ABaseItem*>& InventoryItems = Inventory->GetInventory();
	
	UHealthComponent* HealthComponent = Pawn->FindComponentByClass<UHealthComponent>();

	UStaminaComponent* StaminaComponent = Pawn->FindComponentByClass<UStaminaComponent>();

	if (!HealthComponent || !StaminaComponent)
	{
		return -FLT_MAX;
	}
	
	int32 FoodCount = 0;
	int32 MedkitCount = 0;
	int32 WeaponCount = 0;

	for (ABaseItem* InventoryItem : InventoryItems)
	{
		if (!InventoryItem)
		{
			continue;
		}

		if (Cast<AWeapon>(InventoryItem))
		{
			++WeaponCount;
		}
		else if (Cast<AFood>(InventoryItem))
		{
			++FoodCount;
		}
		else if (Cast<AMedkit>(InventoryItem))
		{
			++MedkitCount;
		}
	}
	float Score = 0.f;

	if (Cast<AMedkit>(Item))
	{
		const float HealthMissing = static_cast<float>(HealthComponent->GetMaxHealth() - HealthComponent->GetHealth());
		Score += 40.f;
		Score += HealthMissing * 20.f;
		Score *= FMath::Pow(0.5f, static_cast<float>(MedkitCount));
	}
	
	else if (Cast<AFood>(Item))
	{
		const float StaminaMissing = StaminaComponent->GetMaxStamina() - StaminaComponent->GetCurrentStamina();
		Score += 40.f;
		Score += StaminaMissing * 15.f;
		Score *= FMath::Pow(0.5f, static_cast<float>(FoodCount));
	}

	else if (Cast<AWeapon>(Item))
	{

		if (WeaponCount == 0)
		{
			Score += 100.f;
		}
		else
		{
			Score += 30.f;
		}

		Score *= FMath::Pow(0.5f, static_cast<float>(WeaponCount));
	}
	else
	{
		return -FLT_MAX;
	}
	
	const float Distance = FVector::Dist2D(Pawn->GetActorLocation(), Item->GetActorLocation());
	
	Score -= Distance * 0.05f;


	return Score;
}


