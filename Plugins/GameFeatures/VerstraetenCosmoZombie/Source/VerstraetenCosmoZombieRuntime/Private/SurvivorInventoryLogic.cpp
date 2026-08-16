#include "SurvivorInventoryLogic.h"

#include "Common/InventoryComponent.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "Items/BaseItem.h"
#include "Items/Food.h"
#include "Items/Medkit.h"
#include "Items/Weapon.h"


USurvivorInventoryLogic::USurvivorInventoryLogic()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USurvivorInventoryLogic::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}

	InventoryComponent = Owner->FindComponentByClass<UInventoryComponent>();

	HealthComponent = Owner->FindComponentByClass<UHealthComponent>();

	StaminaComponent = Owner->FindComponentByClass<UStaminaComponent>();
}

int32 USurvivorInventoryLogic::FindFreeSlot() const
{
	if (!InventoryComponent)
	{
		return INDEX_NONE;
	}

	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		if (!Items[Index])
		{
			return Index;
		}
	}
	
	return INDEX_NONE;
}

bool USurvivorInventoryLogic::IsGarbage(const ABaseItem* Item) const
{
	if (!Item)
	{
		return false;
	}
	
	if (Cast<AFood>(Item))
	{
		return false;
	}

	if (Cast<AMedkit>(Item))
	{
		return false;
	}

	if (Cast<AWeapon>(Item))
	{
		return false;
	}

	return true;
}

bool USurvivorInventoryLogic::IsDepleted(const ABaseItem* Item) const
{
	if (!Item)
	{
		return false;
	}

	return Item->GetValue() <= 0;
}

void USurvivorInventoryLogic::RemoveDepletedItems()
{
	if (!InventoryComponent)
	{
		return;
	}
	
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		ABaseItem* Item = Items[Index];
		
		if (!Item)
		{
			continue;
		}

		if (IsDepleted(Item))
		{
			InventoryComponent->RemoveItem(Index);
		}
	}
}

int32 USurvivorInventoryLogic::CountFood() const
{
	if (!InventoryComponent)
	{
		return 0;
	}
	int32 Count = 0;
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	
	for (ABaseItem* Item : Items)
	{
		if (Cast<AFood>(Item))
		{
			++Count;
		}
	}
	
	return Count;
}

int32 USurvivorInventoryLogic::CountMedkits() const
{
	if (!InventoryComponent)
	{
		return 0;
	}
	
	int32 Count = 0;
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	
	for (ABaseItem* Item : Items)
	{
		if (Cast<AMedkit>(Item))
		{
			++Count;
		}
	}

	return Count;
}

int32 USurvivorInventoryLogic::CountWeapons() const
{
	if (!InventoryComponent)
	{
		return 0;
	}
	
	int32 Count = 0;
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	for (ABaseItem* Item : Items)
	{
		if (Cast<AWeapon>(Item))
		{
			++Count;
		}
	}

	return Count;
}

float USurvivorInventoryLogic::CalculateItemUtility(const ABaseItem* Item, bool bItemAlreadyInInventory) const
{
	if (!Item)
	{
		return -FLT_MAX;
	}

	if (IsGarbage(Item))
	{
		return -FLT_MAX;
	}

	if (IsDepleted(Item))
	{
		return -FLT_MAX;
	}

	if (!HealthComponent || !StaminaComponent)
	{
		return -FLT_MAX;
	}
	
	float Score = 0.f;

	if (Cast<AMedkit>(Item))
	{
		const float MissingHealth = static_cast<float>(HealthComponent->GetMaxHealth() - HealthComponent->GetHealth());

		int32 OtherMedkits = CountMedkits();

		if (bItemAlreadyInInventory)
		{
			--OtherMedkits;
		}
		
		OtherMedkits = FMath::Max(0, OtherMedkits);
		
		Score = 40.f;
		Score += MissingHealth * 20.f;
		Score *= FMath::Pow(0.5f, static_cast<float>(OtherMedkits));
	}
	
	else if (Cast<AFood>(Item))
	{
		const float MissingStamina = StaminaComponent->GetMaxStamina() - StaminaComponent->GetCurrentStamina();

		int32 OtherFood = CountFood();

		if (bItemAlreadyInInventory)
		{
			--OtherFood;
		}
		
		OtherFood = FMath::Max(0, OtherFood);


		Score = 30.f;
		Score += MissingStamina * 15.f;
		
		Score *= FMath::Pow(0.5f,static_cast<float>(OtherFood));
	}
	

	else if (const AWeapon* Weapon = Cast<AWeapon>(Item))
	{
		int32 OtherWeapons = CountWeapons();


		if (bItemAlreadyInInventory)
		{
			--OtherWeapons;
		}


		OtherWeapons = FMath::Max(0, OtherWeapons);
		
		Score = 100.f;

		Score *= FMath::Pow(0.5f,static_cast<float>(OtherWeapons));


		// to prefer pistols
		Score += static_cast<float>(Weapon->GetValue()) * 2.f;
	}
	
	return Score;
}

int32 USurvivorInventoryLogic::FindWorstItemSlot() const
{
	if (!InventoryComponent)
	{
		return INDEX_NONE;
	}
	
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	int32 WorstSlot = INDEX_NONE;
	float WorstScore = FLT_MAX;
	
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		ABaseItem* Item = Items[Index];
		
		if (!Item)
		{
			continue;
		}
		
		const float Score = CalculateItemUtility(Item, true);
		
		if (Score < WorstScore)
		{
			WorstScore = Score;
			WorstSlot = Index;
		}
	}

	return WorstSlot;
}

bool USurvivorInventoryLogic::CanTakeItem(const ABaseItem* Item) const
{
	if (!Item || !InventoryComponent)
	{
		return false;
	}

	if (IsGarbage(Item))
	{
		return FindFreeSlot() != INDEX_NONE;
	}
	
	if (FindFreeSlot() != INDEX_NONE)
	{
		return true;
	}

	// full
	const int32 WorstSlot = FindWorstItemSlot();
	if (WorstSlot == INDEX_NONE)
	{
		return false;
	}
	
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	ABaseItem* WorstItem = Items[WorstSlot];
	const float NewItemScore =CalculateItemUtility(Item,false);
	const float WorstItemScore =CalculateItemUtility(WorstItem,true);
	
	return NewItemScore > WorstItemScore;
}


bool USurvivorInventoryLogic::TryGrabItem(ABaseItem* Item)
{
	if (!Item || !InventoryComponent)
	{
		return false;
	}

	const float Distance =
	FVector::Dist2D(GetOwner()->GetActorLocation(), Item->GetActorLocation());

	if (Distance > InventoryComponent->GetPickupRange())
	{
		return false;
	}
	
	RemoveDepletedItems();
	
	if (IsGarbage(Item))
	{
		const int32 FreeSlot = FindFreeSlot();


		if (FreeSlot == INDEX_NONE)
		{
			return false;
		}
		
		const bool bGrabbed = InventoryComponent->GrabItem(FreeSlot, Item);

		if (!bGrabbed)
		{
			return false;
		}
		InventoryComponent->RemoveItem(FreeSlot);
		
		return true;
	}



	int32 Slot = FindFreeSlot();


	if (Slot != INDEX_NONE)
	{
		return InventoryComponent->GrabItem(Slot, Item);
	}
	
	const int32 WorstSlot = FindWorstItemSlot();


	if (WorstSlot == INDEX_NONE)
	{
		return false;
	}
	
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	ABaseItem* WorstItem = Items[WorstSlot];
	const float NewItemScore = CalculateItemUtility(Item, false);
	const float WorstItemScore =CalculateItemUtility(WorstItem,true);
	
	if (NewItemScore <= WorstItemScore)
	{
		return false;
	}


	if (!InventoryComponent->RemoveItem(WorstSlot))
	{
		return false;
	}
	
	return InventoryComponent->GrabItem(WorstSlot, Item);
}

int32 USurvivorInventoryLogic::FindUsableWeaponSlot() const
{
	if (!InventoryComponent)
	{
				GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Red,
			TEXT("NO INVENTORY COMPONENT"));
		return INDEX_NONE;
	}
	
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		if (IsUsableWeaponSlot(Index))
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

bool USurvivorInventoryLogic::IsUsableWeaponSlot(int32 SlotIndex) const
{
	if (!InventoryComponent)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Red,
			TEXT("InventoryComponent is NULL"));

		return false;
	}
	
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	if (!Items.IsValidIndex(SlotIndex))
	{
		return false;
	}
	ABaseItem* Item = Items[SlotIndex];


	if (!Item)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Yellow,
			FString::Printf(
				TEXT("Slot %d = EMPTY"),
				SlotIndex));

		return false;
	}

	AWeapon* Weapon = Cast<AWeapon>(Items[SlotIndex]);
	if (!Weapon)
	{
		
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Yellow,
			FString::Printf(
				TEXT("Slot %d = %s, NOT WEAPON"),
				SlotIndex,
				*Item->GetName()));
		return false;
	}
	
	GEngine->AddOnScreenDebugMessage(
	-1,
	2.f,
	FColor::Green,
	FString::Printf(
		TEXT("Slot %d = WEAPON %s | Ammo: %d"),
			SlotIndex,
			*Weapon->GetName(),
			Weapon->GetValue()));
	return Weapon->GetValue() > 0;
}


bool USurvivorInventoryLogic::UseItemAndCleanup(int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return false;
	}

	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	
	if (!Items.IsValidIndex(SlotIndex))
	{
		return false;
	}
	
	ABaseItem* Item = Items[SlotIndex];
	if (!Item)
	{
		return false;
	}
	
	if (Item->GetValue() <= 0)
	{
		InventoryComponent->RemoveItem(SlotIndex);
		return false;
	}
	const bool bUsed = InventoryComponent->UseItem(SlotIndex);
	
	if (!bUsed)
	{
		return false;
	}
	
	ABaseItem* ItemAfterUse = Items[SlotIndex];
	
	if (ItemAfterUse && ItemAfterUse->GetValue() <= 0)
	{
		InventoryComponent->RemoveItem(SlotIndex);
	}

	return true;
}

bool USurvivorInventoryLogic::ShouldUseMedkit() const
{
	if (!HealthComponent)
	{
		return false;
	}

	return HealthComponent->GetHealth() <= MedkitHealthThreshold;
}

bool USurvivorInventoryLogic::ShouldUseFood() const
{
	if (!StaminaComponent)
	{
		return false;
	}

	return StaminaComponent->GetCurrentStamina() <= FoodStaminaThreshold;
}

int32 USurvivorInventoryLogic::FindUsableMedkitSlot() const
{
	if (!InventoryComponent)
	{
		return INDEX_NONE;
	}


	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();


	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		AMedkit* Medkit = Cast<AMedkit>(Items[Index]);
		
		if (!Medkit)
		{
			continue;
		}
		
		if (Medkit->GetValue() <= 0)
		{
			continue;
		}
		
		return Index;
	}

	return INDEX_NONE;
}

int32 USurvivorInventoryLogic::FindUsableFoodSlot() const
{
	if (!InventoryComponent)
	{
		return INDEX_NONE;
	}
	
	const TArray<ABaseItem*>& Items = InventoryComponent->GetInventory();
	
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		AFood* Food =
			Cast<AFood>(Items[Index]);
		
		if (!Food)
		{
			continue;
		}
		
		if (Food->GetValue() <= 0)
		{
			continue;
		}
		
		return Index;
	}
	
	return INDEX_NONE;
}