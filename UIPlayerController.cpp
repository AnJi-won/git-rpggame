// Fill out your copyright notice in the Description page of Project Settings.


#include "UIPlayerController.h"
#include "Blueprint/UserWidget.h"

void AUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	MYCHECK(nullptr != UIWidgetClass);

	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
	MYCHECK(nullptr != UIWidgetInstance);

	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}