// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root Component
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	//SphereComp->
	SphereComp->SetVisibility(false,false);
	RootComponent = SphereComp;

	// movement component
	PawnMoveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMoveComp"));
	PawnMoveComp->UpdatedComponent = RootComponent;

	// Spring Arm / Camera
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(-50.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 400.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &ACameraPawn::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &ACameraPawn::MoveRight);

}

void ACameraPawn::MoveForward(float value) {
	FVector forward = GetActorForwardVector();
	const FRotator rot(50.0f, 0.0f, 0.0f);

	AddMovementInput(forward, value * 0.75f);
}

void ACameraPawn::MoveRight(float value) {
	AddMovementInput(GetActorRightVector(), value * 0.75f);
}

