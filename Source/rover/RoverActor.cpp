// Fill out your copyright notice in the Description page of Project Settings.

#include "RoverActor.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
ARoverActor::ARoverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RoverChassisMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chassis"));
	RoverChassisMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> RoverChassisAsset(TEXT("SkeletalMesh'/Game/Objects/MSL/MSL.MSL_Chassis'"));
	
	if (RoverChassisAsset.Succeeded())
	{
		RoverChassisMesh->SetSkeletalMesh(RoverChassisAsset.Object);
		RoverChassisMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	}

	RoverStuffMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Stuff"));
	RoverStuffMesh->SetupAttachment(RoverChassisMesh);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> RoverStuffAsset(TEXT("SkeletalMesh'/Game/Objects/MSL/MSL.MSL_stuff'"));

	if (RoverStuffAsset.Succeeded())
	{
		RoverStuffMesh->SetSkeletalMesh(RoverStuffAsset.Object);
	}
}

// Called when the game starts or when spawned
void ARoverActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

