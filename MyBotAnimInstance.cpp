// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBotAnimInstance.h"

UMyBotAnimInstance::UMyBotAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsDead = false;

	// Attack �ִϸ�Ÿ�� �ε�
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/Book/Animations/Monster_AnimMT.Monster_AnimMT"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}

void UMyBotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// ���� �ӵ� ������ ������ �� CurrentPawnSpeed�� ������Ʈ -> ���� �ӵ��� ���� IDLE / RUN ���°� �����Ƽ� ��Ÿ����.
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
