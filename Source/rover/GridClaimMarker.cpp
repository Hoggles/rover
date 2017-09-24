// Fill out your copyright notice in the Description page of Project Settings.

#include "GridClaimMarker.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"


// Sets default values
AGridClaimMarker::AGridClaimMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root Component
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	//SphereComp->
	SphereComp->SetVisibility(false, false);
	RootComponent = SphereComp;
}

// Called when the game starts or when spawned
void AGridClaimMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridClaimMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

