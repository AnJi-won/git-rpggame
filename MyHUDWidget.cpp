// Fill out your copyright notice in the Description page of Project Settings.
// ¸ó½ºÅÍ HP_Bar UI

#include "MyHUDWidget.h"
#include "MyCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UMyHUDWidget::BindCharacterStat(UMyCharacterStatComponent* NewCharacterStat)
{
	MYCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;
	NewCharacterStat->OnHPChanged.AddUObject(this, &UMyHUDWidget::UpdateHPWidget);
}

void UMyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	MYCHECK(nullptr != HPProgressBar);
	UpdateHPWidget();
}

void UMyHUDWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
