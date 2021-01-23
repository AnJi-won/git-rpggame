// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBotAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AMyBotAIController::HomePosKey(TEXT("HomePos"));
const FName AMyBotAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AMyBotAIController::TargetKey(TEXT("Target"));

AMyBotAIController::AMyBotAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(
		TEXT("/Game/_My/AI/Monster_BB.Monster_BB"));
	if (BBObject.Succeeded())
		BBAsset = BBObject.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(
		TEXT("/Game/_My/AI/Monster_BT.Monster_BT"));
	if (BTObject.Succeeded())
		BTAsset = BTObject.Object;


}

void AMyBotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			LOG(Error, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}

