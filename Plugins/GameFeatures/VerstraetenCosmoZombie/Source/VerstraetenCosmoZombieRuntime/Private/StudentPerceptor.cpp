// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor.h"


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

	if (Stimulus.WasSuccessfullySensed())
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
	}
}
