// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBotAnimInstance.h"

UMyBotAnimInstance::UMyBotAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsDead = false;

	// Attack 애니몽타주 로드
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/Book/Animations/Monster_AnimMT.Monster_AnimMT"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}

void UMyBotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 폰의 속도 정보를 가져온 후 CurrentPawnSpeed에 업데이트 -> 폰의 속도에 따라서 IDLE / RUN 상태가 번갈아서 나타난다.
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}
}

void UMyBotAnimInstance::PlayAttackMontage()
{
	MYCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
}

void UMyBotAnimInstance::AnimNotify_BotAttackCheck()
{
	OnAttackHitCheck.Broadcast();
}
