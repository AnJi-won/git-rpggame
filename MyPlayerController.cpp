// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "PlayerHUDWidget.h"
#include "MyPlayerState.h"
#include "MonsterBot.h"

AMyPlayerController::AMyPlayerController()
{
	static ConstructorHelpers::FClassFinder<UPlayerHUDWidget> UI_HUD_C(
		TEXT("/Game/_My/UI/UI_Player.UI_Player_C"));

	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// UI�� �����ϰ� ���ӿ��Ը� �Է��� �����ϵ��� ���
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	HUDWidget = CreateWidget<UPlayerHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();

	//MyPlayerState = Cast<AMyPlayerState>(PlayerState);
	//MYCHECK(nullptr != MyPlayerState);
	//HUDWidget->BindPlayerState(MyPlayerState);
	//MyPlayerState->OnPlayerStateChanged.Broadcast();
}

UPlayerHUDWidget* AMyPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

//void AMyPlayerController::MonsterKill(AMonsterBot* KilledMonster) const
//{
//	MyPlayerState->AddExp(KilledMonster->GetExp());
//}
