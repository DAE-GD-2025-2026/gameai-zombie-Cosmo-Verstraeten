#include "BTService_LookAround.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"

#include "SurvivorLookAround.h"


UBTService_LookAround::UBTService_LookAround()
{
	NodeName = TEXT("Look Around");

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	
	bNotifyTick = false;
	
}

void UBTService_LookAround::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UE_LOG(
			LogTemp,
			Warning,
			TEXT("LOOK AROUND SERVICE STARTED"));
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return;
	}
	
	APawn* Pawn = AIController->GetPawn();

	if (!Pawn)
	{
		return;
	}
	
	USurvivorLookAround* LookAround = Pawn->FindComponentByClass<USurvivorLookAround>();

	if (!LookAround)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("LookAround: COMPONENT NOT FOUND"));

		return;
	}


	UE_LOG(
		LogTemp,
		Warning,
		TEXT("LookAround: COMPONENT FOUND"));
	LookAround->StartScanning();
}

void UBTService_LookAround::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);


	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return;
	}


	APawn* Pawn = AIController->GetPawn();

	if (!Pawn)
	{
		return;
	}
	
	USurvivorLookAround* LookAround = Pawn->FindComponentByClass<USurvivorLookAround>();

	if (!LookAround)
	{
		return;
	}



	LookAround->StopScanning();
}