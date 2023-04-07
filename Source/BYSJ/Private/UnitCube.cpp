// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCube.h"

// Sets default values
AUnitCube::AUnitCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName{TEXT("UnitCube")});
	SetRootComponent(mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	mesh->SetStaticMesh(MeshAsset.Object);
	//mesh->SetWorldScale3D(FVector(0.01f, 0.01, 0.01f));
	mesh->SetMobility(EComponentMobility::Static);
}

// Called when the game starts or when spawned
void AUnitCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnitCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

