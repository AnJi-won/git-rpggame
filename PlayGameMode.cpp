// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayGameMode.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"

APlayGameMode::APlayGameMode()
{
	// DefaultPawn�� PawnŬ���� ���� ���� 
	DefaultPawnClass = AMyCharacter::StaticClass();

	// �÷��̾� ��Ʈ�ѷ� �Ӽ��� MyPlayerController Ŭ���� ������ ����
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
