// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleGame.h"
#include "GameFramework/GameModeBase.h"
#include "PlayGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGAME_API APlayGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	APlayGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
