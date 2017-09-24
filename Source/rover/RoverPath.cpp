// Fill out your copyright notice in the Description page of Project Settings.

#include "RoverPath.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ARoverPath::ARoverPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("particles"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("ParticleSystem'/Game/Particles/P_Beam.P_Beam'"));

	if (PS.Succeeded()) {
		Particles->SetTemplate(PS.Object);
	}
	
	RootComponent = Particles;
}

// Called when the game starts or when spawned
void ARoverPath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoverPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

