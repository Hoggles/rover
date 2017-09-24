// Fill out your copyright notice in the Description page of Project Settings.

#include "RoverGameMode.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "CameraPawn.h"
#include "RoverHUD.h"
#include "RoverActor.h"
#include "RoverPath.h"
#include "GridClaimMarker.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue,text)
#define error(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,text)
#define LEFT FString(TEXT("L"))
#define RIGHT FString(TEXT("R"))
#define STRAIGHT FString(TEXT("M"))

ARoverGameMode::ARoverGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultPawnClass = ACameraPawn::StaticClass();
	HUDClass = ARoverHUD::StaticClass();

	// ----- WIDGETS -----

	// Mode Select

	static ConstructorHelpers::FClassFinder<UUserWidget> modeSelect_widget(TEXT("/Game/UI/Widgets/mode_select"));

	if (modeSelect_widget.Succeeded()) {
		wModeSelect = modeSelect_widget.Class;
	}
	else {
		wModeSelect = nullptr;
	}

	// Mode 1

	static ConstructorHelpers::FClassFinder<UUserWidget> mode1Input_widget(TEXT("/Game/UI/Widgets/mode1_input"));

	if (mode1Input_widget.Succeeded()) {
		wMode1Input = mode1Input_widget.Class;
	}
	else {
		wMode1Input = nullptr;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> mode1Output_widget(TEXT("/Game/UI/Widgets/mode1_output"));

	if (mode1Output_widget.Succeeded()) {
		wMode1Output = mode1Output_widget.Class;
	}
	else {
		wMode1Output = nullptr;
	}

	// Mode 2

	static ConstructorHelpers::FClassFinder<UUserWidget> mode2Input_widget(TEXT("/Game/UI/Widgets/mode2_input"));

	if (mode2Input_widget.Succeeded()) {
		wMode2Input = mode2Input_widget.Class;
	}
	else {
		wMode2Input = nullptr;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> mode2Output_widget(TEXT("/Game/UI/Widgets/mode2_output"));

	if (mode2Output_widget.Succeeded()) {
		wMode2Output = mode2Output_widget.Class;
	}
	else {
		wMode2Output = nullptr;
	}

	// ----- PARTICLES -----

	static ConstructorHelpers::FObjectFinder<UParticleSystem> beam_particle(TEXT("ParticleSystem'/Game/Particles/P_Beam.P_Beam'"));
	
	if (beam_particle.Succeeded()) {
		BeamParticle = beam_particle.Object;
	}
	else {
		BeamParticle = nullptr;
	}
}

void ARoverGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// mode
	uint32 mode;

	if (!FParse::Value(FCommandLine::Get(), TEXT("mode"), mode)) {
		DisplayModeSelect();
		return;
	}

	// input
	FString inputStr;

	if (FParse::Value(FCommandLine::Get(), TEXT("input"), inputStr)) {
		inputStr = inputStr.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	}
	else {
		DisplayModeSelect();
		return;
	}

	TArray<FString> lines;
	inputStr.ParseIntoArray(lines, TEXT("\n"), true);

	if (lines.Num() < 2) {
		DisplayModeSelect();
		return;
	}

	switch (mode) {
		case 1:
			RunPart1(lines);
			break;

		case 2:
			RunPart2(lines);
			break;

		default:
			DisplayModeSelect();
			break;
	}

}

void ARoverGameMode::DisplayModeSelect() {
	if (wModeSelect)
	{
		auto pcIterator = GetWorld()->GetPlayerControllerIterator();
		APlayerController* player = pcIterator->Get();

		ModeSelectWidget = CreateWidget<UUserWidget>(player, wModeSelect);

		if (ModeSelectWidget)
		{
			ModeSelectWidget->AddToViewport();
			player->bShowMouseCursor = true;
		}

	}
}

void ARoverGameMode::SelectMode(int32 Mode) {
	// We'll remove it from the viewport now.
	// GC will pick it up later.
	if (ModeSelectWidget) {
		ModeSelectWidget->RemoveFromViewport();
	}

	auto pcIterator = GetWorld()->GetPlayerControllerIterator();
	APlayerController* player = pcIterator->Get();

	switch (Mode) {
		case 1:
			if (wMode1Input)
			{
				Mode1InputWidget = CreateWidget<UUserWidget>(player, wMode1Input);

				if (Mode1InputWidget)
				{
					Mode1InputWidget->AddToViewport();
					player->bShowMouseCursor = true;
				}

			}
			break;

		case 2:
			if (wMode2Input)
			{
				Mode2InputWidget = CreateWidget<UUserWidget>(player, wMode2Input);

				if (Mode2InputWidget)
				{
					Mode2InputWidget->AddToViewport();
					player->bShowMouseCursor = true;
				}

			}
			break;

		default:
			DisplayModeSelect();
			break;
	}
}

void ARoverGameMode::RunPart1(TArray<FString> Lines) {
	// If the input widget was used, we'll remove it from the viewport now.
	// GC will pick it up later.
	if (Mode1InputWidget) {
		Mode1InputWidget->RemoveFromViewport();
	}

	AStaticMeshActor* platformMesh = NULL;

	// Setup Plateau 

	TArray<FString> size;
	Lines[0].ParseIntoArray(size, TEXT(" "), true);

	float sizeX = FCString::Atof(*size[0]);
	float sizeY = FCString::Atof(*size[1]);



	// setup each rover

	for (int32 i = 1; i < Lines.Num(); i += 2) {
		TArray<FString> pos;
		Lines[i].ParseIntoArray(pos, TEXT(" "), true);

		float x = FCString::Atof(*pos[0]);
		float y = FCString::Atof(*pos[1]);
		FString direction = pos[2].TrimTrailing();
		float degrees = 0.0f;

		if (direction == FString(TEXT("N"))) {
			degrees = 0.0f;
		}
		else if (direction == FString(TEXT("E"))) {
			degrees = 90.0f;
		}
		else if (direction == FString(TEXT("S"))) {
			degrees = 180.0f;
		}
		else if (direction == FString(TEXT("W"))) {
			degrees = 270.0f;
		}

		FVector location = FVector(y * 100.0f, x * 100.0f, 0.0f); // We need to flip x and y, because Unreal uses a left-handed coordinate system.
		FRotator rotation = FRotator(0.0f, degrees, 0.0f);
		FActorSpawnParameters spawnInfo = FActorSpawnParameters();

		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ARoverActor* spawnedRover = GetWorld()->SpawnActor<ARoverActor>(location, rotation, spawnInfo);

		// TODO: verify that array elements exist
		Rovers.Add(spawnedRover);
		RoverCommands.Add(Lines[i + 1]);
	}

	// Run through commands

	for (int32 roverID = 0; roverID < Rovers.Num(); roverID++) {
		
		FString commands = RoverCommands[roverID].TrimTrailing();

		for (int32 commandID = 0; commandID < commands.Len(); commandID++) {
			TCHAR command = commands[commandID];

			FRotator newDirection = FRotator(0.0f, 0.0f, 0.0f);

			if (command == 'M') {
				MoveRover(Rovers[roverID]);
			}
			else if (command == 'L') {
				newDirection = FRotator(0.0f, -90.0f, 0.0f);
				Rovers[roverID]->AddActorLocalRotation(newDirection);
			}
			else if (command == 'R') {
				newDirection = FRotator(0.0f, 90.0f, 0.0f);
				Rovers[roverID]->AddActorLocalRotation(newDirection);
			}

			//GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMyActor::RepeatingFunction, 1.0f, true, 2.0f);
			
		}
	}

	if (wMode1Output)
	{
		auto pcIterator = GetWorld()->GetPlayerControllerIterator();
		APlayerController* player = pcIterator->Get();

		Mode1OutputWidget = CreateWidget<UUserWidget>(player, wMode1Output);

		if (Mode1OutputWidget)
		{
			Mode1OutputWidget->AddToViewport();
			player->bShowMouseCursor = true;
		}

	}
}

FString ARoverGameMode::GetPart1Output() {

	FString output = FString(TEXT(""));

	for (int32 roverID = 0; roverID < Rovers.Num(); roverID++) {
		FVector location = Rovers[roverID]->GetActorLocation();
		FRotator rotation = Rovers[roverID]->GetActorRotation();

		int32 y = (int32)((location.X + 50) / 100); // We need to flip x and y, because Unreal uses a left-handed coordinate system.
		int32 x = (int32)((location.Y + 50) / 100);

		FString direction = FString(TEXT("N"));

		if (rotation.Yaw == 90.0f || rotation.Yaw == -270.0f) {
			direction = FString(TEXT("E"));
		}
		else if (rotation.Yaw == 180.0f || rotation.Yaw == -180.0f) {
			direction = FString(TEXT("S"));
		}
		else if (rotation.Yaw == 270.0f || rotation.Yaw == -90.0f) {
			direction = FString(TEXT("W"));
		}

		output += FString::FromInt(x) + FString(TEXT(" ")) + FString::FromInt(y) + FString(TEXT(" ")) + direction + FString(TEXT("\n"));
	}

	return output;
}


void ARoverGameMode::RunPart2(TArray<FString> Lines) {
	// If the input widget was used, we'll remove it from the viewport now.
	// GC will pick it up later.
	if (Mode2InputWidget) {
		Mode2InputWidget->RemoveFromViewport();
	}

	// Setup Plateau 

	TArray<FString> size;
	Lines[0].ParseIntoArray(size, TEXT(" "), true);

	float sizeX = FCString::Atof(*size[0]);
	float sizeY = FCString::Atof(*size[1]);

	SetupPlateau(sizeX, sizeY);

	// setup each rover

	for (int32 i = 1; i < Lines.Num(); i++) {
		TArray<FString> pos;
		Lines[i].ParseIntoArray(pos, TEXT(" "), true);

		float x = FCString::Atof(*pos[0]);
		float y = FCString::Atof(*pos[1]);
		FString direction = pos[2].TrimTrailing();
		float degrees = 0.0f;

		if (direction == FString(TEXT("N"))) {
			degrees = 0.0f;
		}
		else if (direction == FString(TEXT("E"))) {
			degrees = 90.0f;
		}
		else if (direction == FString(TEXT("S"))) {
			degrees = 180.0f;
		}
		else if (direction == FString(TEXT("W"))) {
			degrees = 270.0f;
		}

		FVector location = FVector(y * 100.0f, x * 100.0f, 0.0f); // We need to flip x and y, because Unreal uses a left-handed coordinate system.
		FRotator rotation = FRotator(0.0f, degrees, 0.0f);
		FActorSpawnParameters spawnInfo = FActorSpawnParameters();

		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ARoverActor* spawnedRover = GetWorld()->SpawnActor<ARoverActor>(location, rotation, spawnInfo);

		Rovers.Add(spawnedRover);
		RoverCommands.Add(FString(TEXT("")));

		AGridClaimMarker* claim = GetWorld()->SpawnActor<AGridClaimMarker>(location, rotation, spawnInfo);
		claim->Tags.Add(*spawnedRover->GetName());
	}

	// weird way to accomplish multi-dimensional arrays
	struct row
	{
		TArray<bool> columns;
	};

	TArray<row> visited;
	visited.AddDefaulted((int32)sizeY + 1);

	for (int32 i = 0; i < visited.Num(); i++) {
		visited[i].columns.AddDefaulted((int32)sizeX + 1);
	}

	// Run the simulation

	bool complete = false;
	int32 count = 0;
	int32 max = (int32)(sizeX * sizeY * 10); // kind of an arbitrary limit.

	while (!complete) {
		for (int32 i = 0; i < Rovers.Num(); i++) {
			FVector location = Rovers[i]->GetActorLocation();
			FVector forward = Rovers[i]->GetActorForwardVector();
			FVector right = Rovers[i]->GetActorRightVector();

			FVector destination = location + (forward * 100.0f);
			FVector rightDestination = location + (right * 100.0f);
			FVector leftDestination = location + (right * -100.0f);

			// We need to flip x and y, because Unreal uses a left-handed coordinate system.
			int32 straightY = FMath::FloorToInt((destination.X + 50.0f) / 100.0f);
			int32 straightX = FMath::FloorToInt((destination.Y + 50.0f) / 100.0f);
			int32 rightY = FMath::FloorToInt((rightDestination.X + 50.0f) / 100.0f);
			int32 rightX = FMath::FloorToInt((rightDestination.Y + 50.0f) / 100.0f);
			int32 leftY = FMath::FloorToInt((rightDestination.X + 50.0f) / 100.0f);
			int32 leftX = FMath::FloorToInt((rightDestination.Y + 50.0f) / 100.0f);
						
			if (straightY < 0 || straightX < 0 || straightY > sizeY || straightX > sizeX) { 
				// Hit an edge

				FRotator newDirection;

				if (rightX <= 0 || rightY <= 0 || rightX > sizeX || rightY > rightX) {
					newDirection = FRotator(0.0f, -90.0f, 0.0f);
					RoverCommands[i].Append(LEFT);
				}
				else {
					newDirection = FRotator(0.0f, 90.0f, 0.0f);
					RoverCommands[i].Append(RIGHT);
				}

				Rovers[i]->AddActorLocalRotation(newDirection);	
			}
			else {
				// No edge

				AGridClaimMarker* path = NULL;

				int32 selfTraversalTimes = 0;

				for (TActorIterator<AGridClaimMarker> ActorItr(GetWorld()); ActorItr; ++ActorItr)
				{
					if (FVector::Dist(ActorItr->GetActorLocation(), destination) < 25.0f) {
						path = *ActorItr;
						if (path->ActorHasTag(*Rovers[i]->GetName())) {
							selfTraversalTimes++;
						}
					}
				}

				if (path) {
					if (selfTraversalTimes > 0) {
						if (selfTraversalTimes > 1) { // we've traveled this path twice already, look for another path
							if (rightY <= sizeY && rightX <= sizeX && rightY >= 0 && rightX >= 0 && !visited[rightY].columns[rightX]) {
								FRotator newDirection = FRotator(0.0f, 90.0f, 0.0f);
								RoverCommands[i].Append(RIGHT);
								Rovers[i]->AddActorLocalRotation(newDirection);
							}
							else if (leftY <= sizeY && leftX <= sizeX && leftY >= 0 && leftX >= 0 && !visited[leftY].columns[leftX]) {
								FRotator newDirection = FRotator(0.0f, -90.0f, 0.0f);
								RoverCommands[i].Append(LEFT);
								Rovers[i]->AddActorLocalRotation(newDirection);
							} 
							else {
								// no where to really go
								// next rover
								continue;
							}
						}
						else { // we've traveled this once, we'll try to look for a better route, but otherwise use the same.
							if (rightY <= sizeY && rightX <= sizeX && rightY >= 0 && rightX >= 0 && !visited[rightY].columns[rightX]) {
								FRotator newDirection = FRotator(0.0f, 90.0f, 0.0f);
								RoverCommands[i].Append(RIGHT);
								Rovers[i]->AddActorLocalRotation(newDirection);
							}
							else if (leftY <= sizeY && leftX <= sizeX && leftY >= 0 && leftX >= 0 && !visited[leftY].columns[leftX]) {
								FRotator newDirection = FRotator(0.0f, -90.0f, 0.0f);
								RoverCommands[i].Append(LEFT);
								Rovers[i]->AddActorLocalRotation(newDirection);
							}
							else {
								MoveRover(Rovers[i]);
								RoverCommands[i].Append(STRAIGHT);
							}
						}						
					}
					else {
						FRotator newDirection;

						newDirection = FRotator(0.0f, 90.0f, 0.0f);
						RoverCommands[i].Append(RIGHT);

						int32 rand = FMath::RandRange(0, 1);
						if (rand == 0) {
							newDirection = FRotator(0.0f, -90.0f, 0.0f);
							RoverCommands[i].Append(LEFT);
						}
						else {
							newDirection = FRotator(0.0f, 90.0f, 0.0f);
							RoverCommands[i].Append(RIGHT);
						}

						Rovers[i]->AddActorLocalRotation(newDirection);
					}
				}
				else { // New path, continue forth!
					MoveRover(Rovers[i]);
					RoverCommands[i].Append(STRAIGHT);

					visited[straightY].columns[straightX] = true;
				}
			}
		}

		// check if complete
		complete = true;

		for (int32 row = 0; row < visited.Num(); row++) {
			for (int32 column = 0; column < visited[row].columns.Num(); column++) {
				if (visited[row].columns[column] == false) {
					complete = false;
					break;
				}
			}

			if (complete == false) {
				break;
			}
		}

		count++;

		if (count > max) {
			complete = true;
		}
	}

	if (wMode2Output)
	{
		auto pcIterator = GetWorld()->GetPlayerControllerIterator();
		APlayerController* player = pcIterator->Get();

		Mode2OutputWidget = CreateWidget<UUserWidget>(player, wMode2Output);

		if (Mode2OutputWidget)
		{
			Mode2OutputWidget->AddToViewport();
			player->bShowMouseCursor = true;
		}

	}
}

FString ARoverGameMode::GetPart2Output() {

	FString output = FString(TEXT(""));

	for (int32 i = 0; i < RoverCommands.Num(); i++) {
		output += RoverCommands[i] + FString(TEXT("\n"));
	}

	return output;
}

void ARoverGameMode::SetupPlateau(float SizeX, float SizeY) {
	AStaticMeshActor* platformMesh = NULL;

	for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == FString(TEXT("PlateauPlatform"))) {
			platformMesh = *ActorItr;
			break;
		}
	}

	if (!IsValid(platformMesh)) {
		error(TEXT("Could not find the platform!"));
		return;
	}

	platformMesh->SetActorScale3D(FVector(SizeY + 1.0f, SizeX + 1.0f, 1.0f)); // scale starts at 1, not 0, so we need to expand the area by 1 in each coord.
																			  // We need to flip x and y, because Unreal uses a left-handed coordinate system.
}

void ARoverGameMode::MoveRover(ARoverActor* RoverActor) {

	FVector forward = RoverActor->GetActorForwardVector();
	FVector location = RoverActor->GetActorLocation();
	
	FVector destination = location + (forward * 100.0f);

	RoverActor->SetActorLocation(destination);

	FRotator rotation = RoverActor->GetActorRotation();
	FActorSpawnParameters spawnInfo = FActorSpawnParameters();

	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARoverPath* spawnedPath = GetWorld()->SpawnActor<ARoverPath>(location, rotation, spawnInfo);
	spawnedPath->Tags.Add(*RoverActor->GetName());

	AGridClaimMarker* claim = GetWorld()->SpawnActor<AGridClaimMarker>(destination, rotation, spawnInfo);
	claim->Tags.Add(*RoverActor->GetName());
	
	//ARoverPath* path = GetWorld()->SpawnActor<ARoverPath>(destination, rotation, spawnInfo);

	//ACableActor *cable1 = GetWorld()->SpawnActor<ACableActor>(Loc, Rot);

}
