// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Particles/ParticleSystem.h"

#include "RoverActor.h"

#include "RoverGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ROVER_API ARoverGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Rover Mode")
	void SelectMode(int32 Mode);

	UFUNCTION(BlueprintCallable, Category = "Rover Input")
	void RunPart1(TArray<FString> Lines);

	UFUNCTION(BlueprintCallable, Category = "Rover Input")
	void RunPart2(TArray<FString> Lines);

	UFUNCTION(BlueprintCallable, Category = "Rover Output")
	FString GetPart1Output();

	UFUNCTION(BlueprintCallable, Category = "Rover Output")
	FString GetPart2Output();

private:

	UClass * wModeSelect;
	class UUserWidget * ModeSelectWidget;
	UClass * wMode1Input;
	class UUserWidget * Mode1InputWidget;
	UClass * wMode1Output;
	class UUserWidget * Mode1OutputWidget;
	UClass * wMode2Input;
	class UUserWidget * Mode2InputWidget;
	UClass * wMode2Output;
	class UUserWidget * Mode2OutputWidget;
	UParticleSystem* BeamParticle;

	TArray<ARoverActor*> Rovers;
	TArray<FString> RoverCommands;

	virtual void BeginPlay() override;
	
	void DisplayModeSelect();

	void SetupPlateau(float SizeX, float SizeY);

	void MoveRover(ARoverActor* RoverActor);
};
