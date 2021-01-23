// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterStatComponent.h"
#include "MyGameInstance.h"

// Sets default values for this component's properties
UMyCharacterStatComponent::UMyCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
	
	Level = 1;
	MaxMP = 100.f;
	CurrentMP = 100.f;
}


// Called when the game starts
void UMyCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMyCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UMyCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	MYCHECK(nullptr != GameInstance);
	CurrentStatData = GameInstance->GetCharacterData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
	}
	else
	{
		LOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void UMyCharacterStatComponent::SetDamage(float NewDamage)
{
	MYCHECK(nullptr != CurrentStatData);
	
	//SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, MaxHP));
}

void UMyCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}

float UMyCharacterStatComponent::GetAttack()
{
	MYCHECK(nullptr != CurrentStatData, 0.0f);
	//return CurrentStatData->Attack;
	return Attack;
}

float UMyCharacterStatComponent::GetHPRatio()
{
	MYCHECK(nullptr != CurrentStatData, 0.0f);

	//return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
	return (MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / MaxHP);
}

int32 UMyCharacterStatComponent::GetDropExp() const
{
	return CurrentStatData->DropExp;
}

void UMyCharacterStatComponent::SetMP(float NewMP)
{
	CurrentMP = NewMP;
	OnMPChanged.Broadcast();
	if (CurrentMP < KINDA_SMALL_NUMBER)
	{
		CurrentMP = 0.0f;
		OnMPIsZero.Broadcast();
	}
}

float UMyCharacterStatComponent::GetMPRatio()
{
	return (MaxMP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentMP / MaxMP);
}

void UMyCharacterStatComponent::SetMPReduction(float Reduction)
{
	SetMP(FMath::Clamp<float>(CurrentMP - Reduction, 0.0f, MaxMP));
}

void UMyCharacterStatComponent::SetCharacterStat(float _MaxHP, float _MaxMP, float _Attack)
{
	MaxHP = _MaxHP;
	CurrentHP = _MaxHP;

	MaxMP = _MaxMP;
	CurrentMP = _MaxMP;

	Attack = _Attack;
}

void UMyCharacterStatComponent::RecoveryHP(float AddHP)
{
	CurrentHP = CurrentHP + AddHP;
	OnHPChanged.Broadcast();
}



