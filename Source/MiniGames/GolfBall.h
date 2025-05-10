// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <Components/SphereComponent.h>
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GolfPlayerState.h"
#include "Engine/LocalPlayer.h"
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
	virtual void NotifyControllerChanged() override;

public:	

	enum class EGolfState { EIdle, EAiming, EMoving };


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void HandleLookingAround(const FInputActionValue& Value);

	UFUNCTION()
	void CursorOverGolfBall(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void CursorEndOverGolfBall(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void GolfBallAim(const FInputActionValue& Value);

	UFUNCTION()
	void LaunchGolfBall(const FInputActionValue& Value);

	UFUNCTION()
	void GolfBallCameraScroll(const FInputActionValue& Value);

	//Basic Golfball State 
	EGolfState GolfState;

	UFUNCTION()
	void OnBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	//Static Mesh Component
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GolfBall;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* DirectionPointer;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GolfNeutralDisc;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GolfLauncher;

	// Sphere collider component
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollider;

	//Camera Component
	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraSpringArm;

	//Camera Component
	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	//Input Mapping Context
	UPROPERTY(EditAnywhere, Category = "Enhanced Input")
	UInputMappingContext* DefaultMappingContext;

	//Input action for looking around
	UPROPERTY(EditAnywhere, Category = "Enhanced Input")
	UInputAction* LookAround;

	//Input action for hitting golf ball
	UPROPERTY(EditAnywhere, Category = "Enhanced Input")
	UInputAction* HitGolfBall;

	//Input action for rotation golf ball 
	UPROPERTY(EditAnywhere, Category = "Enhanced Input")
	UInputAction* AimGolfBall;

	//Input action for zooming in and out on mouse wheel
	UPROPERTY(EditAnywhere, Category = "Enhanced Input")
	UInputAction* CameraScroll;

	UPROPERTY()
	APlayerController* GolfController;

	//Golf ball impact value
	UPROPERTY(EditAnywhere)
	float Speed;

	UPROPERTY(EditAnywhere)
	float MouseAimingStartPosition;

};
