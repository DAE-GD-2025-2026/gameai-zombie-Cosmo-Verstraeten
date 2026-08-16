#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorInventoryLogic.generated.h"


class ABaseItem;
class AWeapon;

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VERSTRAETENCOSMOZOMBIERUNTIME_API USurvivorInventoryLogic : public UActorComponent
{
	GENERATED_BODY()

public:

	USurvivorInventoryLogic();

protected:

	virtual void BeginPlay() override;

public:

	int32 FindFreeSlot() const;

	int32 FindUsableWeaponSlot() const;
	
	bool IsGarbage(const ABaseItem* Item) const;

	bool IsDepleted(const ABaseItem* Item) const;
	
	float CalculateItemUtility(const ABaseItem* Item,bool bItemAlreadyInInventory) const;

	int32 FindWorstItemSlot() const;

	bool CanTakeItem(const ABaseItem* Item) const;

	bool TryGrabItem(ABaseItem* Item);

	bool UseItemAndCleanup(int32 SlotIndex);

	void RemoveDepletedItems();


private:

	int32 CountFood() const;
	int32 CountMedkits() const;
	int32 CountWeapons() const;


	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent{};

	UPROPERTY()
	TObjectPtr<UHealthComponent> HealthComponent{};

	UPROPERTY()
	TObjectPtr<UStaminaComponent> StaminaComponent{};
};