// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RoverHUD.generated.h"

/**
 * 
 */
UCLASS()
class ROVER_API ARoverHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
	
private:

	UClass * wControls;
	class UUserWidget * ControlsWidget;
	
	virtual void BeginPlay() override;
	
};
