// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "MyGameInstance.h"


AMyPlayerState::AMyPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
	Exp = 0;
}

int32 AMyPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AMyPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

float AMyPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		return 0.0f;

	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	LOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);

	return Result;
}

bool AMyPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
		return false;

	bool DidLevelUp = false;
	Exp = Exp + IncomeExp;
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();

	return DidLevelUp;
}

void AMyPlayerState::InitPlayerData()
{
	SetPlayerName(TEXT("Player"));
	SetCharacterLevel(5);
	GameScore = 0;
	Exp = 0;
}

void AMyPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	MYCHECK(nullptr != MyGameInstance);

	CurrentStatData = MyGameInstance->GetCharacterData(NewCharacterLevel);
	MYCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}
