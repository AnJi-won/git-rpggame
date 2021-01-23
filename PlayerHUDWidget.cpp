// Fill out your copyright notice in the Description page of Project Settings.
// 플레이어 UI 관리

#include "PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MyCharacterStatComponent.h"
#include "MyPlayerState.h"


void UPlayerHUDWidget::BindCharacterStat(class UMyCharacterStatComponent* CharacterStat)
{
	MYCHECK(nullptr != CharacterStat);
	CurrentCharacterStat = CharacterStat;
	CharacterStat->OnHPChanged.AddUObject(this, &UPlayerHUDWidget::UpdateCharacterStat);

	
}

//void UPlayerHUDWidget::BindPlayerState(AMyPlayerState* PlayerState)
//{
//	MYCHECK(nullptr != PlayerState);
//	CurrentPlayerState = PlayerState;
//	PlayerState->OnPlayerStateChanged.AddUObject(this, &UPlayerHUDWidget::UpdateCharacterState);
//}

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	MYCHECK(nullptr != HPBar);

	//ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbExp")));
	//MYCHECK(nullptr != ExpBar);

	/*PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	MYCHECK(nullptr != PlayerName);

	PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
	MYCHECK(nullptr != PlayerLevel);*/

	/*CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore")));
	MYCHECK(nullptr != CurrentScore);

	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore")));
	MYCHECK(nullptr != HighScore);*/

	MPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_MPBar")));
	MYCHECK(nullptr != MPBar);
}

void UPlayerHUDWidget::UpdateCharacterStat()
{
	MYCHECK(CurrentCharacterStat.IsValid());

	HPBar->SetPercent(CurrentCharacterStat->GetHPRatio());
	MPBar->SetPercent(CurrentCharacterStat->GetMPRatio());
}

//void UPlayerHUDWidget::UpdateCharacterState()
//{
//	MYCHECK(CurrentPlayerState.IsValid());
//
//	ExpBar->SetPercent(CurrentPlayerState->GetExpRatio());
//	//PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
//	//PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));
//	//CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));
//}
