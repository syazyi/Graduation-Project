// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildPart/BuildComponent.h"

#include "MyPawn.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UBuildComponent::UBuildComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	m_MaterialsForPreview.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/BYSJ/BuildPreviewGreen.BuildPreviewGreen'")).Object);
	m_MaterialsForPreview.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/BYSJ/BuildPreviewRed.BuildPreviewRed'")).Object);
	m_MaterialsForPreview.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/BYSJ/BuildPreviewGray.BuildPreviewGray'")).Object);

	FBuildType BuildTypeFoundation;
	BuildTypeFoundation.Mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Fantastic_Village_Pack/meshes/buildings/SM_BLD_base_v01_01.SM_BLD_base_v01_01'")).Object;
	BuildTypeFoundation.CollisionChannel = ECC_EngineTraceChannel1;
	BuildTypeFoundation.Actor = ConstructorHelpers::FClassFinder<AActor>(TEXT("Blueprint'/Game/BYSJ/Build/Foundation.Foundation_C'")).Class;
	m_Builds.Add(EBuildType::eBuildFoundation, BuildTypeFoundation);

	FBuildType BuildTypeLight;
	BuildTypeLight.Mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Fantastic_Village_Pack/meshes/props/light/SM_PROP_lantern_01.SM_PROP_lantern_01'")).Object;
	BuildTypeLight.CollisionChannel = ECC_EngineTraceChannel2;
	BuildTypeLight.Actor = ConstructorHelpers::FClassFinder<AActor>(TEXT("Blueprint'/Game/BYSJ/Build/Light.Light_C'")).Class;
	m_Builds.Add(EBuildType::eBuildLight, BuildTypeLight);

	FBuildType BuildTypeBoard;
	BuildTypeBoard.Mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Fantastic_Village_Pack/meshes/props/deco/SM_PROP_board_01.SM_PROP_board_01'")).Object;
	BuildTypeBoard.CollisionChannel = ECC_EngineTraceChannel3;
	BuildTypeBoard.Actor = ConstructorHelpers::FClassFinder<AActor>(TEXT("Blueprint'/Game/BYSJ/Build/Board.Board_C'")).Class;
	m_Builds.Add(EBuildType::eBuildBoard, BuildTypeBoard);
}


// Called when the game starts
void UBuildComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	m_PlayerRef = Cast<AMyPawn>(GetOwner());
	GetWorld()->GetTimerManager().SetTimer(m_BuildSystemTimerHandle, this, &UBuildComponent::BuildCycle, 0.5f, true, 0.1f);
	GetWorld()->GetTimerManager().PauseTimer(m_BuildSystemTimerHandle);
}


// Called every frame
void UBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBuildComponent::SetCamera(UCameraComponent* camera)
{
	m_Camera = camera;
}

void UBuildComponent::SpawnBuildPreview()
{
	m_BuildPreview = Cast<UStaticMeshComponent>(m_PlayerRef->AddComponentByClass(UStaticMeshComponent::StaticClass(), false, m_BuildTransForm, false));
	m_BuildPreview->SetStaticMesh(m_Builds[m_CurrentBuild].Mesh);
	m_BuildPreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UBuildComponent::GiveBuildColor(bool isGreen)
{
	m_CanBuild = true;
	if(isGreen)
	{
		m_BuildPreview->SetMaterial(0, m_MaterialsForPreview[0]);
	}
	else
	{
		m_BuildPreview->SetMaterial(0, m_MaterialsForPreview[1]);
	}
	m_BuildPreview->SetWorldTransform(m_BuildTransForm);
}

void UBuildComponent::BuildCycle()
{
	auto cameraLocation = m_Camera->GetComponentLocation();
	auto cameraFrontVec = m_Camera->GetForwardVector();
	auto endBuildPoint =  cameraFrontVec * 5000 + cameraLocation;
	auto startBuildPoint = cameraFrontVec * 350 + cameraLocation;

	FHitResult hitResult;
	bool hitGround = GetWorld()->LineTraceSingleByChannel(hitResult, startBuildPoint, endBuildPoint,ECC_Visibility);
	//UE_LOG(LogTemp, Warning, TEXT("%d"), hitGround);
	m_CanBeBuild = hitGround;
	if(hitGround)
	{
		m_BuildTransForm.SetLocation(hitResult.Location);
	}else
	{
		m_BuildTransForm.SetLocation(endBuildPoint);
	}
	if(!m_BuildPreview)
	{
		SpawnBuildPreview();
	}
	GiveBuildColor(hitGround);
	BuildDelay();
}

void UBuildComponent::BuildDelay()
{
	GetWorld()->GetTimerManager().UnPauseTimer(m_BuildSystemTimerHandle);
}

void UBuildComponent::StopBuildMode()
{
	m_IsBuildModeOn = false;
	m_CanBuild = false;
	m_BuildPreview->DestroyComponent();
	m_BuildPreview = nullptr;
	GetWorld()->GetTimerManager().PauseTimer(m_BuildSystemTimerHandle);
}

void UBuildComponent::LaunchBuildMode()
{
	if(m_IsBuildModeOn)
	{
		StopBuildMode();
	}
	else
	{
		m_IsBuildModeOn = true;
		BuildCycle();
	}
}

void UBuildComponent::PutBuild()
{
	if(m_IsBuildModeOn && m_CanBeBuild)
	{
		AActor* actor = GetWorld()->SpawnActor(m_Builds[m_CurrentBuild].Actor, &m_BuildTransForm);
		//m_PlayerRef->GetLevel()->Actors.Add(actor);
	}
}

void UBuildComponent::ChangeBuild(EBuildType buildType)
{
	m_CurrentBuild = buildType;
	m_BuildPreview->DestroyComponent();
	m_BuildPreview = nullptr;
}


