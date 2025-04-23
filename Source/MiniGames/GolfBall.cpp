// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBall.h"

// Sets default values
AGolfBall::AGolfBall()
{

	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Set up the mesh for the golf ball
	GolfBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = GolfBall;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/EditorMeshes/ArcadeEditorSphere.ArcadeEditorSphere'"));
	if (MeshAsset.Succeeded())
	{
		GolfBall->SetStaticMesh(MeshAsset.Object);
	}

	// Enable physics for the golf ball
	GolfBall->SetSimulatePhysics(true);
	GolfBall->SetEnableGravity(true);
	GolfBall->SetRelativeScale3D(FVector(.1f, .1f, .1f));

	//Setup direction pointer 
	DirectionPointer = CreateDefaultSubobject<UStaticMeshComponent>("DirectionComponent");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetCone(TEXT("StaticMesh'/Engine/BasicShapes/Cone'"));

	if (MeshAssetCone.Succeeded())
	{
		DirectionPointer->SetStaticMesh(MeshAssetCone.Object);
	}

	DirectionPointer->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	DirectionPointer->SetRelativeLocation(FVector(290.0f, GolfBall->K2_GetComponentLocation().Y, GolfBall->K2_GetComponentLocation().Z));
	DirectionPointer->SetCollisionProfileName("OverlapAll");
	DirectionPointer->SetVisibility(false);
	DirectionPointer->bCastStaticShadow = false;
	DirectionPointer->SetupAttachment(RootComponent);


	GolfNeutralDisc = CreateDefaultSubobject<UStaticMeshComponent>("GolfNeutralDisc");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetDisc(TEXT("StaticMesh'/Game/MiniGames/Levels/_GENERATED/Kaleb/Disc_0BB4F872.Disc_0BB4F872'"));

	if (MeshAssetDisc.Succeeded())
	{
		GolfNeutralDisc->SetStaticMesh(MeshAssetDisc.Object);
	}

	//GolfNeutralDisc->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	GolfNeutralDisc->SetRelativeLocation(FVector(0.0f, 0.0f, GolfBall->Bounds.SphereRadius * -1.0f));
	GolfNeutralDisc->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
	GolfNeutralDisc->SetCollisionProfileName("OverlapAll");
	GolfNeutralDisc->bCastStaticShadow = false;
	GolfNeutralDisc->SetupAttachment(RootComponent);

	//Setup golf launcher 
	GolfLauncher = CreateDefaultSubobject<UStaticMeshComponent>("LauncherComponent");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetLauncher(TEXT("StaticMesh'/Game/MiniGames/Levels/_GENERATED/Kaleb/Torus_E9BEF269.Torus_E9BEF269'"));

	if (MeshAssetLauncher.Succeeded())
	{
		GolfLauncher->SetStaticMesh(MeshAssetLauncher.Object);
	}

	GolfLauncher->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	GolfLauncher->SetRelativeLocation(FVector(GolfBall->K2_GetComponentLocation().X, GolfBall->K2_GetComponentLocation().Y, GolfBall->K2_GetComponentLocation().Z));
	GolfLauncher->SetCollisionProfileName("OverlapAll");
	GolfLauncher->bCastStaticShadow = false;
	GolfLauncher->SetupAttachment(RootComponent);




	// Create the Sphere Collider and set it as the root component
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereCollider->SetupAttachment(RootComponent);
	SphereCollider->SetSphereRadius(100.0f);

	// Create Camera Boon (spring arm) and attach it to the SphereCollider (not the root)
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraSpringArm->TargetArmLength = 400.0f;
	CameraSpringArm->AddLocalRotation(FQuat(FRotator(331.0f, 180.0f, 0.0f)));
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->bInheritPitch = false;
	CameraSpringArm->bInheritYaw = false;
	CameraSpringArm->bInheritRoll = false;
	CameraSpringArm->SetupAttachment(RootComponent);

	// Create the camera and attach it to the CameraBoon (spring arm)
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(CameraSpringArm);

	//Posses player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//Set Golf State
	GolfState = EGolfState::EIdle;

	Speed = 100.0f;
	MouseAimingStartPosition = 0.0f;



}

// Called when the game starts or when spawned
void AGolfBall::BeginPlay()
{
	Super::BeginPlay();

	GolfBall->OnBeginCursorOver.AddDynamic(this, &AGolfBall::CursorOverGolfBall);
	GolfBall->OnEndCursorOver.AddDynamic(this, &AGolfBall::CursorEndOverGolfBall);

	FRotator SpringArmRotation = CameraSpringArm->GetComponentRotation();

	// Set the new relative rotation for the mesh component
	//GolfBall->AddRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	GolfController = Cast<APlayerController>(Controller);


}

// Called every frame
void AGolfBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GolfController) {
		float MouseX;
		float MouseY;
		FVector MouseWorld;
		FVector MouseDirection;

		GolfController->GetMousePosition(MouseX, MouseY);

		GolfController->DeprojectMousePositionToWorld(MouseWorld, MouseDirection);

		FVector GolfBallPos = GolfBall->GetComponentLocation();

		float MouseDistanceFromGolfBall = GolfBallPos.Y - MouseWorld.Y;


		FVector GolfBallCurrentVelocity = GolfBall->GetComponentVelocity();

		if (GolfState != EGolfState::EAiming && FMath::Abs(GolfBallCurrentVelocity.X) < 0.1)
		{
			GolfState = EGolfState::EIdle;
			GolfBall->SetRelativeRotation(FRotator(0.0f, CameraSpringArm->GetRelativeRotation().Yaw, 0.0f));
			//GolfController->CurrentMouseCursor = EMouseCursor::Default;

		}

		if (GolfState == EGolfState::EIdle)
		{
			GolfNeutralDisc->SetVisibility(true);
		}
		else if (GolfState == EGolfState::EMoving)
		{
			GolfNeutralDisc->SetVisibility(false);
		}


	}


	
	if(GolfController)
	{

		FHitResult Hit;

		// We set up a line trace from our current location to a point 1000cm ahead of us
		FVector MouseWorld;
		FVector MouseDirection;

		GolfController->DeprojectMousePositionToWorld(MouseWorld, MouseDirection);
		FVector TraceStart = MouseWorld;
		FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 1000.0f;

		// You can use FCollisionQueryParams to further configure the query
		// Here we add ourselves to the ignored list so we won't block the trace
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);


		// To run the query, you need a pointer to the current level, which you can get from an Actor with GetWorld()
	//UWorld()->LineTraceSingleByChannel runs a line trace and returns the first actor hit over the provided collision channel.
		GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_EngineTraceChannel1, QueryParams);

		// You can use DrawDebug helpers and the log to help visualize and debug your trace queries.
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 0.2f);
	
	}

}

void AGolfBall::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGolfBall::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAround, ETriggerEvent::Triggered, this, &AGolfBall::HandleLookingAround);
		EnhancedInputComponent->BindAction(HitGolfBall, ETriggerEvent::Completed, this, &AGolfBall::LaunchGolfBall);
		EnhancedInputComponent->BindAction(AimGolfBall, ETriggerEvent::Triggered, this, &AGolfBall::GolfBallAim);
		EnhancedInputComponent->BindAction(CameraScroll, ETriggerEvent::Triggered, this, &AGolfBall::GolfBallCameraScroll);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGolfBall::HandleLookingAround(const FInputActionValue& Value)
{
	FVector2D MouseAxisVector = Value.Get<FVector2D>();

	FRotator SpringArmRotation = FRotator(MouseAxisVector.Y * 1.0f, MouseAxisVector.X * 1.0f, 0.0f);

	if (GolfState == EGolfState::EIdle)
	{
		CameraSpringArm->AddRelativeRotation(SpringArmRotation);
	}
}

void AGolfBall::CursorOverGolfBall(UPrimitiveComponent* TouchedComponent)
{

	if (GolfController)
	{
		GolfController->CurrentMouseCursor = EMouseCursor::GrabHand;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Mouse Cursor Change"));
	}

	if (GolfState == EGolfState::EIdle && GolfController && GolfController->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		GolfState = EGolfState::EAiming;
		FVector MouseWorld;
		FVector MouseDirection;

		GolfController->DeprojectMousePositionToWorld(MouseWorld, MouseDirection);

		FVector GolfBallPos = GolfBall->GetComponentLocation();

		FVector MouseDistanceFromGolfBall = GolfBallPos - MouseWorld;
		MouseAimingStartPosition = MouseDistanceFromGolfBall.Z;
	}

}

void AGolfBall::CursorEndOverGolfBall(UPrimitiveComponent* TouchedComponent)
{

	if (GolfController && GolfController->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		GolfState = EGolfState::EAiming;
	}
	else
	{
		GolfState = EGolfState::EIdle;
	}
}


void AGolfBall::GolfBallAim(const FInputActionValue& Value)
{

	FVector2D MouseAxisVector = Value.Get<FVector2D>();
	FHitResult HitResult;


	if (GolfController)
	{
		//GolfController->CurrentMouseCursor = EMouseCursor::GrabHandClosed;
		GolfController->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);
		FRotator GolfBallRotation = FRotator(0.0f, MouseAxisVector.X * -2.0f, 0.0f);
		AActor* HitActor = HitResult.GetActor();
		FVector GolfBallCurrentVelocity = GolfBall->GetComponentVelocity();
		FVector MouseWorld;
		FVector MouseDirection;

		GolfController->DeprojectMousePositionToWorld(MouseWorld, MouseDirection);

		FVector GolfBallPos = GolfBall->GetComponentLocation();

		FVector MouseDistanceFromGolfBall = GolfBallPos - MouseWorld;



		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(MouseAimingStartPosition));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::SanitizeFloat(FMath::Abs(MouseDistanceFromGolfBall.Y)));



		if (GolfState == EGolfState::EAiming && FMath::Abs(GolfBallCurrentVelocity.X) < 0.5)
		{
			DirectionPointer->SetVisibility(true);
			GolfBall->AddRelativeRotation(GolfBallRotation);
			GolfLauncher->SetRelativeLocation(FVector(MouseWorld.Y * 5.0f, GolfBall->GetComponentLocation().Y, GolfBall->GetComponentLocation().Z));

		}

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::SanitizeFloat(MouseWorld.Z));

	}

}

void AGolfBall::LaunchGolfBall(const FInputActionValue& Value)
{
	FVector GolfBallCurrentVelocity = GolfBall->GetComponentVelocity();

	if (GolfState == EGolfState::EAiming && FMath::Abs(GolfBallCurrentVelocity.X) < 0.5) {
		FVector GolfBallForwardVect = GolfBall->GetForwardVector() * 20000.0f;
		GolfBall->AddImpulse(GolfBallForwardVect);
	}

	GolfState = EGolfState::EMoving;
	DirectionPointer->SetVisibility(false);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::SanitizeFloat(MouseDistanceFromGolfBall.Z));
}

void AGolfBall::GolfBallCameraScroll(const FInputActionValue& Value)
{
	FVector2D ZoomAxis = Value.Get<FVector2D>();

	if (ZoomAxis.X == 1 && CameraSpringArm->TargetArmLength > 400.0f)
	{
		CameraSpringArm->TargetArmLength += -50.0f;
	}
	else if (ZoomAxis.X == -1 && CameraSpringArm->TargetArmLength < 3000.0f)
	{
		CameraSpringArm->TargetArmLength += 50.0f;
	}

	//FString TargetArmLength = FString::SanitizeFloat(CameraSpringArm->TargetArmLength);
}

void AGolfBall::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
}


