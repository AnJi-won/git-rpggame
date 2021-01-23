// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleGame.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGAME_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMyPlayerController();

	class UPlayerHUDWidget* GetHUDWidget() const;
	//void MonsterKill(class AMonsterBot* KilledMonster) const;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UPlayerHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
		class UPlayerHUDWidget* HUDWidget;

	//UPROPERTY()
	//	class AMyPlayerState* MyPlayerState;
};
