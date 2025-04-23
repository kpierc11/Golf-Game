// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfPlayerController.h"


AGolfPlayerController::AGolfPlayerController()
{
	bEnableClickEvents = true;
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
}

void AGolfPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void AGolfPlayerController::Tick(float DeltaTime)
{
}

void AGolfPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

}

