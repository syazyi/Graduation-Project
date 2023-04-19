// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain/TerrainHelper.h"

#include "NoiseMapGernerator.h"

// Sets default values
ATerrainHelper::ATerrainHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ATerrainHelper::BeginPlay()
{
	Super::BeginPlay();
	GenerateTerrain();
}

// Called every frame
void ATerrainHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATerrainHelper::GenerateTerrain()
{
	auto World = GetWorld();
	if(World)
	{
		NoiseMapGernerators.Add(World->SpawnActor<ANoiseMapGernerator>(GetActorLocation(), FRotator()));
	}
}

