// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GolfPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AGolfPlayerState : public APlayerState
{
	GENERATED_BODY()



public:

	AGolfPlayerState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	float GolfHitCount = 0.0f;

	UFUNCTION()
	float GetGolfHitCount() const { return GolfHitCount; }

	UFUNCTION()
	void SetGolfHitCount(float HitCount) { GolfHitCount = HitCount; }
	
};
