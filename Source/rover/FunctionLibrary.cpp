// Fill out your copyright notice in the Description page of Project Settings.

#include "FunctionLibrary.h"


TArray<FString> UFunctionLibrary::ParseInput(FString Input) {
	TArray<FString> lines;
	Input.ParseIntoArray(lines, TEXT("\n"), false);
	
	return lines;
}


