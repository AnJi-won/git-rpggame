// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDoor.h"
#include "MyCharacter.h"

// Sets default values
AMyDoor::AMyDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	RootComponent = Mesh;
	Trigger->SetBoxExtent(FVector(30.0f, 100.0f, 300.0f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.f, 50.f, 0.f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMyDoor::OnTriggerBeginOverlap);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DOOR_MESH(
		TEXT("/Game/Book/StaticMesh/SM_GATE.SM_GATE"));
	if (DOOR_MESH.Succeeded())
	{
		Mesh->SetStaticMesh(DOOR_MESH.Object);
	}

	IsOpen = false;
}

// Called when the game starts or when spawned
void AMyDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyDoor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (false == IsOpen && OtherActor->IsA(AMyCharacter::StaticClass()))
	{
		SetActorRelativeRotation(GetActorRotation() + FRotator(0.f, -90.f, 0.f));
		IsOpen = true;
	}
	
}

