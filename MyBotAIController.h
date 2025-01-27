// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleGame.h"
#include "AIController.h"
#include "MyBotAIController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGAME_API AMyBotAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMyBotAIController();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	
private:
	UPROPERTY()
		class UBehaviorTree* BTAsset;

	UPROPERTY()
		class UBlackboardData* BBAsset;
};
