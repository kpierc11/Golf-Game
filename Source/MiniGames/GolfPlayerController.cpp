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

	InputComponent->BindAction("MouseClick", IE_Pressed, this, &AGolfPlayerController::GolfBallClicked);

}


void AGolfPlayerController::GolfBallClicked()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);

	if (HitResult.GetComponent())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Mouse Click+++ Component: %s"), *HitResult.GetComponent()->GetName()));
	}


	
}
