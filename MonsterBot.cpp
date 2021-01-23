// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBot.h"
#include "MyBotAIController.h"
#include "MyBotAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "MyCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
#include "MyHUDWidget.h"
#include "MyPlayerController.h"
#include "MyCharacter.h"

// Sets default values
AMonsterBot::AMonsterBot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMyBotAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CharacterStat = CreateDefaultSubobject<UMyCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	DeadEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DEADEFFECT"));
	

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(
		TEXT("/Game/_My/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
		HPBarWidget->SetHiddenInGame(true);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	// ½ºÄÌ·¹Å» ¸Þ½Ã ºÒ·¯¿À±â
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PAWN(
		TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_FrostGiant.SK_CharM_FrostGiant"));
	if (SK_PAWN.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_PAWN.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(
		TEXT("/Game/Book/Animations/MonsterAnimBP.MonsterAnimBP_C"));
	if (WARRIOR_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	IsAttacking = false;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Monster"));
	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	DeadTimer = 2.0f;

	CharacterStat->SetNewLevel(1);

	SetupWeapon();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_DEAD_EFFECT(
		TEXT("/Game/InfinityBladeEffects/Effects/FX_Combat_Ice/P_Player_Frozen_Exit_01.P_Player_Frozen_Exit_01"));
	if (P_DEAD_EFFECT.Succeeded())
	{
		DeadEffect->SetTemplate(P_DEAD_EFFECT.Object);
		DeadEffect->SetupAttachment(GetCapsuleComponent());
		DeadEffect->bAutoActivate = false;
	}

	/*AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	if (AudioComp)
	{
		AudioComp->SetupAttachment(RootComponent);
		
	}
	static ConstructorHelpers::FObjectFinder<USoundWave> sound(TEXT("/Game/_My/Sound/07_Main.07_Main"));
	if (sound.Succeeded())
	{
		AudioComp->SetSound(sound.Object);
		AudioComp->bAutoActivate = false;
	}*/

	DeadSound = CreateDefaultSubobject<UAudioComponent>(TEXT("DeadSound"));
	DeadSound->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USoundWave> DEAD_SOUND(TEXT("/Game/_My/Sound/Los_BossDead_01_gate_f.Los_BossDead_01_gate_f"));
	if (DEAD_SOUND.Succeeded())
	{
		DeadSound->SetSound(DEAD_SOUND.Object);
		DeadSound->bAutoActivate = false;
	}

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	Trigger->SetupAttachment(GetMesh());
	Trigger->SetBoxExtent(FVector(130.f, 130.f, 300.f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMonsterBot::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AMonsterBot::OnTriggerEndOverlap);
}

int32 AMonsterBot::GetExp() const
{
	return CharacterStat->GetDropExp();
}

// Called when the game starts or when spawned
void AMonsterBot::BeginPlay()
{
	Super::BeginPlay();

	auto CharacterWidget = Cast<UMyHUDWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget)
	{
		CharacterWidget->BindCharacterStat(CharacterStat);
	}

	BotController = Cast<AMyBotAIController>(GetController());
	MYCHECK(nullptr != BotController);
	
}

// Called every frame
void AMonsterBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AMonsterBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	BotAnim = Cast<UMyBotAnimInstance>(GetMesh()->GetAnimInstance());
	MYCHECK(nullptr != BotAnim);

	BotAnim->OnMontageEnded.AddDynamic(this, &AMonsterBot::OnAttackMontageEnded);
	BotAnim->OnAttackHitCheck.AddUObject(this, &AMonsterBot::AttackCheck);
	
	CharacterStat->SetCharacterStat(100.f, 0.f, 10.f);
	CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
		LOG(Warning, TEXT("OnHPIsZero"));
		BotAnim->SetDeadAnim();
		SetActorEnableCollision(false);

		IsDead = true;
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			Destroy();
		}), DeadTimer, false);

		DeadSound->Play();
	});

	
}

float AMonsterBot::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	LOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	
	CharacterStat->SetDamage(FinalDamage);
	
	
	if (IsDead && EventInstigator->IsPlayerController())
	{
		AMyPlayerController* playerController = Cast<AMyPlayerController>(EventInstigator);
		MYCHECK(nullptr != playerController, 0.0f);
		//playerController->MonsterKill(this);
		
		PlayDeadEffect();
	}
	return FinalDamage;
}

// Called to bind functionality to input
void AMonsterBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AMonsterBot::Attack()
{
	if (false == IsAttacking)
	{
		BotAnim->PlayAttackMontage();
		IsAttacking = true;
	}
	
}

void AMonsterBot::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	MYCHECK(IsAttacking);
	IsAttacking = false;
	OnAttackEnd.Broadcast();
}

void AMonsterBot::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

//#if ENABLE_DRAW_DEBUG
//
//	FVector TraceVec = GetActorForwardVector() * AttackRange;
//	FVector Center = GetActorLocation() + TraceVec * 0.5f;
//	float HalfHeight = AttackRange * 0.5f + AttackRadius;
//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
//	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
//	float DebugLifeTime = 5.0f;
//
//	DrawDebugCapsule(GetWorld(),
//		Center,
//		HalfHeight,
//		AttackRadius,
//		CapsuleRot,
//		DrawColor,
//		false,
//		DebugLifeTime);
//
//#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			//LOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);

			
		}
	}
	
}

void AMonsterBot::SetupWeapon()
{
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(
			TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_Glimmer/SK_Blade_Glimmer.SK_Blade_Glimmer"));
		if (SK_WEAPON.Succeeded())
		{
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
}

void AMonsterBot::PlayDeadEffect()
{
	if(IsDead)
		DeadEffect->Activate(false);
}

void AMonsterBot::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AMyCharacter::StaticClass()))
	{
		HPBarWidget->SetHiddenInGame(false);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Collision Begin!!"));
	}
}

void AMonsterBot::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AMyCharacter::StaticClass()))
	{
		HPBarWidget->SetHiddenInGame(true);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Collision End!!"));
	}
}

