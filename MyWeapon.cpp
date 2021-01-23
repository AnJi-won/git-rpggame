// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

// Sets default values
AMyWeapon::AMyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = Weapon;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(
		TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
	if (SK_WEAPON.Succeeded())
		Weapon->SetSkeletalMesh(SK_WEAPON.Object);

	Weapon->SetCollisionProfileName(TEXT("NoCollision"));

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	Effect->SetupAttachment(RootComponent);
	Effect->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_EFFECT(
		TEXT("/Game/InfinityBladeEffects/Effects/FX_Combat_Base/Impact/P_Impact_Enemy_Fire_Strong.P_Impact_Enemy_Fire_Strong"));
	if (P_EFFECT.Succeeded())
	{
		Effect->SetTemplate(P_EFFECT.Object);
		Effect->bAutoActivate = false;
		
	}
}

// Called when the game starts or when spawned
void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyWeapon::ActivateEffect()
{
	Effect->Activate();
}


