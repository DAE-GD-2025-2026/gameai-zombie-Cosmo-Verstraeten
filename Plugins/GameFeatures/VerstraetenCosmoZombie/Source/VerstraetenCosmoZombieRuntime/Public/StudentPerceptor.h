// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "StudentPerceptor.generated.h"

class ABaseItem;
class ABaseZombie;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VERSTRAETENCOSMOZOMBIERUNTIME_API UStudentPerceptor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStudentPerceptor();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	const TArray<TObjectPtr<ABaseItem>>& GetKnownItems() const
	{
		return KnownItems;
	}
	
	const TArray<TObjectPtr<AActor>>& GetKnownHouses() const
	{
		return KnownHouses;
	}


	bool IsHouseSearched(AActor* House) const
	{
		return SearchedHouses.Contains(House);
	}
	
	bool HasUnsearchedHouse() const;

	
	void MarkHouseSearched(AActor* House)
	{
		if (!House)
		{
			return;
		}

		SearchedHouses.AddUnique(House);
	}
	
	void ForgetItem(ABaseItem* Item);

	const TArray<TObjectPtr<ABaseZombie>>& GetVisibleEnemies() const
	{
		return VisibleEnemies;
	}

	
private:
	
	void DebugPrintKnownItems() const;
	
	void HandleZombiePerception(ABaseZombie* Zombie, const FAIStimulus& Stimulus);

	UPROPERTY()
	TArray<TObjectPtr<ABaseItem>> KnownItems{};
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> KnownHouses{};

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SearchedHouses{};
	
	UPROPERTY()
	TArray<TObjectPtr<ABaseZombie>> VisibleEnemies{};

};
