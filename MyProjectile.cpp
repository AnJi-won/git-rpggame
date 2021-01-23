// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectile.h"
#include "MonsterBot.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMyProjectile::AMyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->InitSphereRadius(200.f);
	RootComponent = SphereCollision;
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyProjectile::OnTriggerBeginOverlap);

	Sound = CreateDefaultSubobject<UAudioComponent>(TEXT("Sound"));
	static ConstructorHelpers::FObjectFinder<USoundWave> SOUND(
		TEXT("/Game/_My/Sound/SE_FIRE_ELEMENTS_002.SE_FIRE_ELEMENTS_002"));
	if (SOUND.Succeeded())
	{
		Sound->SetSound(SOUND.Object);
		Sound->bAutoActivate = false;
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	ProjectileMovement->SetUpdatedComponent(SphereCollision);
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 3.f;
	

	InitialLifeSpan = 3.0f;

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	Effect->SetupAttachment(SphereCollision);
	Effect->bAutoActivate = true;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_EFFECT(
		TEXT("/Game/InfinityBladeEffects/Effects/FX_Skill_Leap/P_Skill_Leap_Fire_Impact_Suction.P_Skill_Leap_Fire_Impact_Suction"));
	if (P_EFFECT.Succeeded())
	{
		Effect->SetTemplate(P_EFFECT.Object);
		
	}

	Damage = 500.f;

	EffectTimer = 0.5f;
}

// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	Sound->Play();
	
	GetWorld()->GetTimerManager().SetTimer(EffectTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
		Effect->Activate();
	}), EffectTimer, false);


}

// Called every frame
void AMyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AMyProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}

void AMyProjectile::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AMonsterBot::StaticClass()))
	{
		FDamageEvent DamageEvent;
		OtherActor->TakeDamage(500.f, DamageEvent, OtherActor->GetInstigatorController(), this);
		
	}
}

