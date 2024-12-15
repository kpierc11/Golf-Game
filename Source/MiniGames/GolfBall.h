// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <Components/SphereComponent.h>
#include "GolfBall.generated.h"




UCLASS()
class MINIGAMES_API AGolfBall : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGolfBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Camera Component
	UPROPERTY(EditAnywhere)
	AActor* CameraOne;

	//Static Mesh Component
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GolfBall;

	// Sphere collider component
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollider;


};
