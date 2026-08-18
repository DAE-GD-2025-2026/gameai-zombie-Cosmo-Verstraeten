#include "BTTask_ShootZombie.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Survivor/SurvivorPawn.h"
#include "Zombies/BaseZombie.h"

#include "StudentPerceptor.h"
#include "SurvivorInventoryLogic.h"


UBTTask_ShootZombie::UBTTask_ShootZombie()
{
	NodeName = TEXT("Shoot Zombie");
	bNotifyTick = true;
	bCreateNodeInstance = true;
	
	TargetEnemyKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_ShootZombie,
			TargetEnemyKey),
		AActor::StaticClass());
	
	WeaponSlotKey.AddIntFilter(
		this,
		GET_MEMBER_NAME_CHECKED(
			UBTTask_ShootZombie,
			WeaponSlotKey));
}

EBTNodeResult::Type UBTTask_ShootZombie::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	
	GEngine->AddOnScreenDebugMessage(
	-1,
	2.f,
	FColor::Cyan,
	TEXT("SHOOT EXECUTE"));
	
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}


	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(Controller->GetPawn());
	if (!Survivor)
	{
		return EBTNodeResult::Failed;
	}


	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}


	ABaseZombie* Zombie = Cast<ABaseZombie>(Blackboard->GetValueAsObject(TargetEnemyKey.SelectedKeyName));
	if (!IsValid(Zombie))
	{
		return EBTNodeResult::Failed;
	}


	USurvivorInventoryLogic* InventoryLogic =Survivor->FindComponentByClass<USurvivorInventoryLogic>();
	if (!InventoryLogic)
	{
		return EBTNodeResult::Failed;
	}


	const int32 WeaponSlot = Blackboard->GetValueAsInt(WeaponSlotKey.SelectedKeyName);
	if (!InventoryLogic->IsUsableWeaponSlot(WeaponSlot))
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTask_ShootZombie::TickTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}
	
	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(Controller->GetPawn());

	if (!Survivor)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}


	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Blackboard)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}


	ABaseZombie* Zombie =Cast<ABaseZombie>(Blackboard->GetValueAsObject(TargetEnemyKey.SelectedKeyName));

	if (!IsValid(Zombie))
	{
		GEngine->AddOnScreenDebugMessage(
	-1,
	2.f,
	FColor::Red,
	TEXT("SHOOT FAIL: enemy invalid"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		return;
	}
	
	UStudentPerceptor* Perceptor = Survivor->FindComponentByClass<UStudentPerceptor>();
	if (!Perceptor)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}
	
	bool bZombieStillVisible = false;
	const TArray<TObjectPtr<ABaseZombie>>& VisibleZombies = Perceptor->GetVisibleEnemies();
	
	for (const TObjectPtr<ABaseZombie>& VisibleZombie : VisibleZombies)
	{
		if (VisibleZombie.Get() == Zombie)
		{
			bZombieStillVisible = true;
			break;
		}
	}


	if (!bZombieStillVisible)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
		return;
	}
	
	FVector ToZombie = Zombie->GetActorLocation() - Survivor->GetActorLocation();
	ToZombie.Z = 0.f;
	
	if (ToZombie.IsNearlyZero())
	{
		return;
	}

	ToZombie.Normalize();
	const float CurrentYaw = Survivor->GetActorRotation().Yaw;
	const float DesiredYaw = ToZombie.Rotation().Yaw;
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw);
	
	if (FMath::Abs(DeltaYaw) > AimToleranceDegrees)
	{
		const float AngularVelocity = FMath::Sign(DeltaYaw) * MaxAngularVelocity;
		
		const float RotationThisFrame = AngularVelocity * DeltaSeconds;
		
		Survivor->AddActorWorldRotation(FRotator(0.f, RotationThisFrame, 0.f));
		
		return;
	}
	
	USurvivorInventoryLogic* InventoryLogic = Survivor->FindComponentByClass<USurvivorInventoryLogic>();
	if (!InventoryLogic)
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
		return;
	}
	
	const int32 WeaponSlot =Blackboard->GetValueAsInt(WeaponSlotKey.SelectedKeyName);
	
	if (!InventoryLogic->IsUsableWeaponSlot(WeaponSlot))
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
		return;
	}

	InventoryLogic->UseItemAndCleanup(WeaponSlot);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}