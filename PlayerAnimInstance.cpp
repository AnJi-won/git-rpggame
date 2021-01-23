// Fill out your copyright notice in the Description page of Project Settings.

// 플레이어 애니메이션을 관리하는 애님 인스턴스 클래스
#include "PlayerAnimInstance.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	// Attack 콤보 애니몽타주 로드
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/Book/Animations/Warrior_AnimMT.Warrior_AnimMT"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;

	// Skill 콤보 애니몽타주 로드
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

// 애님 인스턴스에서 Tick마다 호출되는 함수이다.
void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 폰의 속도 정보를 가져온 후 CurrentPawnSpeed에 업데이트 -> 폰의 속도에 따라서 IDLE / RUN 상태가 번갈아서 나타난다.
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			// IsInAir : 캐릭터가 현재 공중에 떠 있는지 아닌지에 대한 정보를 보관한다.
			// -> 해당 변수는 애니메이션 블루프린트에서 점프 애니메이션을 설계할 때 사용한다.
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
	// 멀티캐스트 델리게이트 OnAttackHitCheck에 등록된 함수들을 모두 호출한다.
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
