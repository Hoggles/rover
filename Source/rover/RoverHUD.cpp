// Fill out your copyright notice in the Description page of Project Settings.

#include "RoverHUD.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

ARoverHUD::ARoverHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> controls_widget(TEXT("/Game/UI/Widgets/controls"));

	if (controls_widget.Succeeded()) {
		wControls = controls_widget.Class;
	}
	else {
		wControls = nullptr;
	}

}

void ARoverHUD::BeginPlay() {
	Super::BeginPlay();

	if (wControls)
	{
		auto pcIterator = GetWorld()->GetPlayerControllerIterator();
		APlayerController* player = pcIterator->Get();

		ControlsWidget = CreateWidget<UUserWidget>(player, wControls);

		if (ControlsWidget)
		{
			ControlsWidget->AddToViewport();
		}
		else {
			// TODO: Print error to screen
		}

	}
}
