// Fill out your copyright notice in the Description page of Project Settings.

// �÷��̾� �ִϸ��̼��� �����ϴ� �ִ� �ν��Ͻ� Ŭ����
#include "PlayerAnimInstance.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	// Attack �޺� �ִϸ�Ÿ�� �ε�
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/Book/Animations/Warrior_AnimMT.Warrior_AnimMT"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;

	// Skill �޺� �ִϸ�Ÿ�� �ε�
	static ConstructorHelpers::FObjectFinder<UAnimMontage> SKILL_MONTAGE(
		TEXT("/Game/Book/Animations/Skill_AnimMT.Skill_AnimMT"));
	if (SKILL_MONTAGE.Succeeded())
		SkillMontage = SKILL_MONTAGE.Object;

	Sound_Combo = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundCombo"));
	
	static ConstructorHelpers::FObjectFinder<USoundWave> SOUND_COMBO(
		TEXT("/Game/_My/Sound/SSword_Swing_01.SSword_Swing_01"));
	if (SOUND_COMBO.Succeeded())
	{
		Sound_Combo->SetSound(SOUND_COMBO.Object);
		Sound_Combo->bAutoActivate = false;
	}

	

}

// �ִ� �ν��Ͻ����� Tick���� ȣ��Ǵ� �Լ��̴�.
void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// ���� �ӵ� ������ ������ �� CurrentPawnSpeed�� ������Ʈ -> ���� �ӵ��� ���� IDLE / RUN ���°� �����Ƽ� ��Ÿ����.
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			// IsInAir : ĳ���Ͱ� ���� ���߿� �� �ִ��� �ƴ����� ���� ������ �����Ѵ�.
			// -> �ش� ������ �ִϸ��̼� �������Ʈ���� ���� �ִϸ��̼��� ������ �� ����Ѵ�.
			IsInAir = Character->GetCharacterMovement()->IsFalling();
		}
	}
		
}

void UPlayerAnimInstance::PlayAttackMontage()
{
	MYCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
}

void UPlayerAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	MYCHECK(!IsDead);
	MYCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UPlayerAnimInstance::PlaySkillAnimation()
{
	MYCHECK(!IsDead);
	Montage_Play(SkillMontage, 1.0f);
}

void UPlayerAnimInstance::AnimNotify_AttackHitCheck()
{
	Sound_Combo->Play();
	// ��Ƽĳ��Ʈ ��������Ʈ OnAttackHitCheck�� ��ϵ� �Լ����� ��� ȣ���Ѵ�.
	OnAttackHitCheck.Broadcast();
}

void UPlayerAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UPlayerAnimInstance::AnimNotify_ComboEffectCheck()
{
	OnComboEffectCheck.Broadcast();
}

void UPlayerAnimInstance::AnimNotify_Skill()
{
	OnSkillCheck.Broadcast();
}

FName UPlayerAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	MYCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
