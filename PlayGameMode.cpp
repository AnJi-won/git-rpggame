// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayGameMode.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"

APlayGameMode::APlayGameMode()
{
	// DefaultPawn에 Pawn클래스 정보 지정 
	DefaultPawnClass = AMyCharacter::StaticClass();

	// 플레이어 컨트롤러 속성을 MyPlayerController 클래스 값으로 변경
	PlayerControllerClass = AMyPlayerController::StaticClass();

	PlayerStateClass = AMyPlayerState::StaticClass();
}

void APlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto MyPlayerState = Cast<AMyPlayerState>(NewPlayer->PlayerState);
	MYCHECK(nullptr != MyPlayerState);
	MyPlayerState->InitPlayerData();
}
