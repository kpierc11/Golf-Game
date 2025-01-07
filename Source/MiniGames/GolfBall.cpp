// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBall.h"

// Sets default values
AGolfBall::AGolfBall()
{

    // Set this actor to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = true;

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

    // Create the Sphere Collider and set it as the root component
    SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereCollider->SetupAttachment(RootComponent);
    SphereCollider->SetSphereRadius(100.0f);   

    // Create Camera Boon (spring arm) and attach it to the SphereCollider (not the root)
    CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    CameraSpringArm->TargetArmLength = 1983.713257;
    CameraSpringArm->AddLocalRotation(FQuat(FRotator(331.0f, 180.0f, 0.0f)));
    CameraSpringArm->SetupAttachment(RootComponent);

    // Create the camera and attach it to the CameraBoon (spring arm)
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    Camera->SetupAttachment(CameraSpringArm);

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    Speed = 100.0f;
    bIsGolfBallHit = false;
    bIsAimingGolfBall = false;

}

// Called when the game starts or when spawned
void AGolfBall::BeginPlay()
{
	Super::BeginPlay();
    
    GolfBall->OnBeginCursorOver.AddDynamic(this, &AGolfBall::CursorOverGolfBall);
    GolfBall->OnEndCursorOver.AddDynamic(this, &AGolfBall::CursorEndOverGolfBall);

    FRotator NewRotation = FRotator(0.0f,0.0f,0.0f);

    // Set the new relative rotation for the mesh component
    GolfBall->SetWorldRotation(NewRotation);

    //Lock Golfball in place when game starts
    FBodyInstance* BodyInstance = GolfBall->GetBodyInstance();
    //BodyInstance->bLockXTranslation = true;
   // BodyInstance->bLockYTranslation = true;
    //BodyInstance->bLockZTranslation = true;
    //BodyInstance->SetDOFLock(EDOFMode::SixDOF);

}

// Called every frame
void AGolfBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    FHitResult Hit;
    FVector TraceStart = GolfBall->GetComponentLocation();
    FRotator WorldRotation = GolfBall->GetComponentRotation();
    FVector TraceEnd = (GolfBall->GetForwardVector() * 500.0f) + TraceStart;
    FHitResult Result;
    FCollisionQueryParams QueryParams;

    QueryParams.AddIgnoredActor(this);

    GetWorld()->LineTraceSingleByChannel(Result, TraceStart, TraceEnd, ECC_Pawn, QueryParams);

    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Green, false, 5.0f, 0, 10.0f);

    //FVector CurrentGolfBallVelocity = GolfBall->GetComponentVelocity();

    //UE_LOG(LogTemp, Warning, TEXT("Components Velocity->>>: X: %.2f, Y: %.2f, Z: %.2f"), CurrentGolfBallVelocity.X, CurrentGolfBallVelocity.Y, CurrentGolfBallVelocity.Z);


   /* if (IsGolfBallHit && CurrentGolfBallVelocity.X <= 4.0f && CurrentGolfBallVelocity.Z != 0.0f)
    {
        BodyInstance->bLockXTranslation = true;
        BodyInstance->bLockYTranslation = true;
        BodyInstance->SetDOFLock(EDOFMode::SixDOF);
        IsGolfBallHit = false;
    }*/

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
    APlayerController* PlayerController = Cast<APlayerController>(Controller);
    FRotator SpringArmRotation = FRotator(MouseAxisVector.Y * 1.0f, MouseAxisVector.X * 1.0f, 0.0f);

    if (!PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
    {
        bIsAimingGolfBall = false;
    }

    if (!bIsAimingGolfBall)
    {
        CameraSpringArm->AddRelativeRotation(SpringArmRotation);
    }

    //GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Looking around "));
}

void AGolfBall::CursorOverGolfBall(UPrimitiveComponent* TouchedComponent)
{
    bIsAimingGolfBall = true;
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Cursor over Actor. "));
}

void AGolfBall::CursorEndOverGolfBall(UPrimitiveComponent* TouchedComponent)
{
    APlayerController* PlayerController = Cast<APlayerController>(Controller);
    if (PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
    {
        bIsAimingGolfBall = true;
    }
    else {
        bIsAimingGolfBall = false;
    }
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Cursor not over actor. "));
}


void AGolfBall::GolfBallAim(const FInputActionValue& Value)
{
    FVector2D MouseAxisVector = Value.Get<FVector2D>();
    FHitResult HitResult;
    APlayerController* PlayerController = Cast<APlayerController>(Controller);
    PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);
    FRotator GolfBallRotation = FRotator(0.0f, MouseAxisVector.X * -5.0f, 0.0f);
    AActor* HitActor = HitResult.GetActor();

    if (bIsAimingGolfBall)
    {
       GolfBall->AddRelativeRotation(GolfBallRotation);
    }
}

void AGolfBall::LaunchGolfBall(const FInputActionValue& Value)
{

    if (bIsAimingGolfBall) {
        FVector GolfBallForwardVect = GolfBall->GetForwardVector() * Speed;

        GolfBall->AddForce(GolfBallForwardVect);
    }

    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Launced Golf Ball"));
}

void AGolfBall::GolfBallCameraScroll(const FInputActionValue& Value)
{
    FVector2D ZoomAxis = Value.Get<FVector2D>();

    if (ZoomAxis.X == 1)
    {
        CameraSpringArm->TargetArmLength += -50.0f;
    }
    else if (ZoomAxis.X == -1)
    {
        CameraSpringArm->TargetArmLength += 50.0f;
    }

}

