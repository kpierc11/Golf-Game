// Fill out your copyright notice in the Description page of Project Settings.


#include "GolfBall.h"

// Sets default values
AGolfBall::AGolfBall()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//Set Default Mesh for GolfBall
	GolfBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = GolfBall;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/EditorMeshes/ArcadeEditorSphere.ArcadeEditorSphere'"));
	if (MeshAsset.Succeeded())
	{
		GolfBall->SetStaticMesh(MeshAsset.Object);
	}

	// Create the sphere collider component
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(GolfBall);  // Attach the sphere collider to the mesh component

	// Set the radius of the sphere collider (change to suit your needs)
	SphereCollider->SetSphereRadius(100.0f);

	//// Set collision profile for the sphere collider
	SphereCollider->SetCollisionProfileName(TEXT("BlockAll"));

	// Optionally, enable collision events (e.g., overlap or hit events)
	SphereCollider->SetGenerateOverlapEvents(true);

	GolfBall->SetSimulatePhysics(true);

	GolfBall->SetEnableGravity(true);

	GolfBall->SetMassOverrideInKg(NAME_None, 300.0f);
}

// Called when the game starts or when spawned
void AGolfBall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGolfBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGolfBall::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

