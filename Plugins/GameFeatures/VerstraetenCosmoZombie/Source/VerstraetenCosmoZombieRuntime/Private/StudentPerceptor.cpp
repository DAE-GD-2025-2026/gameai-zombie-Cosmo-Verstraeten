// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Village/House/House.h"
#include "Items/BaseItem.h"


UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	/*GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));*/
	
	/*GEngine->AddOnScreenDebugMessage(
	-1,
	2.f,
	FColor::Green,
	FString::Printf(TEXT("Perceived: %s"), *Actor->GetName()));*/
	
	
	if (!Actor)
	{
		return;
	}
	
	if (!Stimulus.WasSuccessfullySensed())
	{
		return;
	}

	ABaseItem* Item = Cast<ABaseItem>(Actor);
	
	if (Item)
	{
		KnownItems.AddUnique(Item);

		DebugPrintKnownItems();
	}
	
	APawn* Pawn = Cast<APawn>(GetOwner());

	if (!Pawn)
	{
		return;
	}

	AAIController* AIController =
		Cast<AAIController>(Pawn->GetController());

	if (!AIController)
	{
		return;
	}
	
	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
	
	if (!Blackboard)
	{
		return;
	}
	
	if (!Actor->IsA<AHouse>())
	{
		return;
	}
	
	KnownHouses.AddUnique(Actor);
	
	Blackboard->SetValueAsBool(TEXT("HasKnownHouse"),true);
	
	GEngine->AddOnScreenDebugMessage(
	-1,
	3.f,
	FColor::Green,
	FString::Printf(
		TEXT("Remembered house: %s | Known Houses: %d"),
		*Actor->GetName(),
		KnownHouses.Num()
	)
);
	
	/*if (Stimulus.WasSuccessfullySensed())
	{
		
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			FString::Printf(TEXT("Detected: %s"), *Actor->GetName())
		);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Red,
			FString::Printf(TEXT("Lost: %s"), *Actor->GetName())
		);
	}*/
}


void UStudentPerceptor::ForgetItem(ABaseItem* Item)
{
	if (!Item)
	{
		return;
	}

	KnownItems.Remove(Item);
}


void UStudentPerceptor::DebugPrintKnownItems() const
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Yellow,
		FString::Printf(
			TEXT("Known Items: %d"),
			KnownItems.Num()
		)
	);

	for (const TObjectPtr<ABaseItem>& Item : KnownItems)
	{
		if (Item)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Green,
				FString::Printf(
					TEXT(" - %s"),
					*Item->GetName()
				)
			);
		}
	}
}

bool UStudentPerceptor::HasUnsearchedHouse() const
{
	for (const TObjectPtr<AActor>& House : KnownHouses)
	{
		if (!House)
		{
			continue;
		}

		if (!SearchedHouses.Contains(House))
		{
			return true;
		}
	}

	return false;
}