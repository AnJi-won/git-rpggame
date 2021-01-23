// Fill out your copyright notice in the Description page of Project Settings.
// 캐릭터 스탯 데이터를 관리한다.

#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
	FString CharacterDataPath = TEXT("/Game/_My/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_CHARACTER(*CharacterDataPath);
	MYCHECK(DT_CHARACTER.Succeeded());
	CharacterTable = DT_CHARACTER.Object;
	MYCHECK(CharacterTable->GetRowMap().Num() > 0);
}

void UMyGameInstance::Init()
{
	Super::Init();
	
}

FCharacterData* UMyGameInstance::GetCharacterData(int32 Level)
{
	return CharacterTable->FindRow< FCharacterData>(*FString::FromInt(Level), TEXT(""));
}
