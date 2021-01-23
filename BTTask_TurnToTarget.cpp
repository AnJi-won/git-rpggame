// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "MyBotAIController.h"
#include "MonsterBot.h"
#include "MyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
    NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto MyBot = Cast<AMonsterBot>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == MyBot)
        return EBTNodeResult::Failed;

    if(MyBot->Get_IsDead())
        return EBTNodeResult::Failed;

    auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMyBotAIController::TargetKey));
    if (nullptr == Target)
        return EBTNodeResult::Failed;

    FVector LookVector = Target->GetActorLocation() - MyBot->GetActorLocation();
    LookVector.Z = 0.f;
    FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
    MyBot->SetActorRotation(FMath::RInterpTo(MyBot->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));

    return EBTNodeResult::Succeeded;
}
