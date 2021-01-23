// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleGame.h"
#include "Animation/AnimInstance.h"
#include "MyBotAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnBotAttackCheckDelegate);

UCLASS()
class BATTLEGAME_API UMyBotAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMyBotAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void SetDeadAnim() { IsDead = true; }

	FOnBotAttackCheckDelegate OnAttackHitCheck;

private:
	// ��Ƽ���� üũ �Լ�
	UFUNCTION()
		void AnimNotify_BotAttackCheck();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		float CurrentPawnSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsDead;
};
