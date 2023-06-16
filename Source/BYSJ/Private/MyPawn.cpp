// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Net/UnrealNetwork.h"
#include "NoiseMapGernerator.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent =  StaticMesh;
	StaticMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'")).Object);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetStaticMesh(nullptr);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArmComponent);

	BuildComponent = CreateDefaultSubobject<UBuildComponent>("Build System");
	bReplicates = true;
}

void AMyPawn::DestroyComponent()
{
	BuildComponent->DestroyComponent();
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent->MaxSpeed = Speed;
	BuildComponent->SetCamera(CameraComponent);
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateCurrentBlockLocation();
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMyPawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyPawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Build", IE_Pressed, BuildComponent, &UBuildComponent::LaunchBuildMode);
	PlayerInputComponent->BindAction("PutBuild", IE_Pressed, BuildComponent, &UBuildComponent::PutBuild);
	PlayerInputComponent->BindAction<ChangeBuildDelegate>("Build1", IE_Pressed, BuildComponent, &UBuildComponent::ChangeBuild, EBuildType::eBuildFoundation);
	PlayerInputComponent->BindAction<ChangeBuildDelegate>("Build2", IE_Pressed, BuildComponent, &UBuildComponent::ChangeBuild, EBuildType::eBuildBoard);
	PlayerInputComponent->BindAction<ChangeBuildDelegate>("Build3", IE_Pressed, BuildComponent, &UBuildComponent::ChangeBuild, EBuildType::eBuildLight);
}

void AMyPawn::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector(), value * Speed);
}

void AMyPawn::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector(), value * Speed);
}

void AMyPawn::CalculateCurrentBlockLocation()
{
	auto Transform = GetTransform();
	auto Location = Transform.GetLocation();
	//25000 = width * meshscale / 2;
	int k = ANoiseMapGernerator::width * ANoiseMapGernerator::MeshScale;
	int d = k / 2;
	int XID = static_cast<int>(Location.X + d * FMath::Sign(Location.X)) / k;
	int YID = static_cast<int>(Location.Y + d * FMath::Sign(Location.Y)) / k;

	CurrentLocation = FVector(XID * k, YID * k, 0.0f);
}

/*void AMyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}*/
