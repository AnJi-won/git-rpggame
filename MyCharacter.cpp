// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "PlayerAnimInstance.h"
#include "MyWeapon.h"
#include "MyCharacterStatComponent.h"
//#include "DrawDebugHelpers.h"
//#include "Components/WidgetComponent.h"
//#include "MyHUDWidget.h"
#include "MyPlayerController.h"
#include "PlayerHUDWidget.h"
#include "MyPlayerState.h"
#include "MyProjectile.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UMyCharacterStatComponent>(TEXT("CHARACTERSTAT"));


	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyCharacter"));

	// 스켈레탈 메시 불러오기
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PAWN(
		TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_solid.SK_CharM_solid"));
	if (SK_PAWN.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_PAWN.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(
		TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);

	SetControlMode();
	IsAttacking = false;
	MaxCombo = 4;
	AttackEndComboState();


	AttackRange = 200.0f;
	AttackRadius = 50.0f;


	ProjectileClass = AMyProjectile::StaticClass();
	
	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FName WeaponSocket(TEXT("hand_rSocket"));
	MyWeapon = GetWorld()->SpawnActor<AMyWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (MyWeapon)
	{
		MyWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}

	MyPlayerController = Cast<AMyPlayerController>(GetController());
	MYCHECK(nullptr != MyPlayerController);
	MyPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

	CharacterStat->SetCharacterStat(1000.f, 100.f, 30.f);
	//auto MyPlayerState = Cast<AMyPlayerState>(GetPlayerState());
	//MYCHECK(nullptr != MyPlayerState);
	//CharacterStat->SetNewLevel(MyPlayerState->GetCharacterLevel());
	CharacterStat->OnHPChanged.Broadcast();

	
}

void AMyCharacter::SetControlMode()
{
	SpringArm->TargetArmLength = 1000.f;
	SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	MYCHECK(nullptr != PlayerAnim);

	// 애님 인스턴스의 OnMontageEnded 델리게이트와 OnAttackMontageEnded 함수를 연결해서
	// 델리게이트가 발동할 때까지 애니메이션 시스템에 몽타주 재생 명령을 내리지 못하게 막는다.
	PlayerAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);

	PlayerAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		//LOG(Warning, TEXT("OnNextAttackCheck"));

		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			PlayerAnim->JumpToAttackMontageSection(CurrentCombo);
			
		}
	});
	
	PlayerAnim->OnAttackHitCheck.AddUObject(this, &AMyCharacter::AttackCheck);

	PlayerAnim->OnComboEffectCheck.AddLambda([this]() -> void {
		MyWeapon->ActivateEffect();
	});

	PlayerAnim->OnSkillCheck.AddLambda([this]() -> void {

		
		SpawnSkill();
	});

	

	CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
		LOG(Warning, TEXT("OnHPIsZero"));
		PlayerAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	LOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);
	return FinalDamage;
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AMyCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Recovery"), EInputEvent::IE_Pressed, this, &AMyCharacter::Recovery);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &AMyCharacter::Fire);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);
}

void AMyCharacter::UpDown(float NewAxisValue)
{
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
}

void AMyCharacter::LeftRight(float NewAxisValue)
{
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
}

void AMyCharacter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void AMyCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void AMyCharacter::Attack()
{
	if (IsAttacking)
	{
		MYCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		
		if (CanNextCombo)
		{
			IsComboInputOn = true;
			
		}
	}
	else
	{
		MYCHECK(CurrentCombo == 0);
		AttackStartComboState();
		
		PlayerAnim->PlayAttackMontage();
		PlayerAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
		
	}
}

void AMyCharacter::Recovery()
{
	CharacterStat->SetMPReduction(10);
	CharacterStat->RecoveryHP(200);
	
}

void AMyCharacter::Fire()
{
	PlayerAnim->PlaySkillAnimation();
	
}

void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 애니메이션 몽타주 동작이 끝났을 때 호출
	MYCHECK(IsAttacking);
	MYCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AMyCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	MYCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AMyCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AMyCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
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

void AMyCharacter::SpawnSkill()
{
	if (ProjectileClass)
	{
		// MuzzleOffset 을 카메라 스페이스에서 월드 스페이스로 변환합니다.

		FVector Location = GetActorLocation() + GetActorForwardVector();

		FRotator Rotation = GetActorRotation();
		
		UWorld* World = GetWorld();
		if (World)
		{
			/*FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			AMyProjectile* Projectile = World->SpawnActor<AMyProjectile>(ProjectileClass, Location, Rotation, SpawnParams);
			Projectile->FireInDirection(GetActorForwardVector());*/

			for (int i = 0; i < 10; i++)
			{
				int32 SpawnX = (rand() % 10 + 1) * 50;

				const FRotator SpawnRotation = GetActorRotation();
				const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(FVector(80.f + SpawnX, 0.f, -80.f));

				AMyProjectile* Projectile = World->SpawnActor<AMyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
				Projectile->FireInDirection(GetActorForwardVector());
			}
			
		}

		

		
	}
}



