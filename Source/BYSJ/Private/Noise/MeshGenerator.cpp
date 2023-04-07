// Fill out your copyright notice in the Description page of Project Settings.


#include "Noise/MeshGenerator.h"

// Sets default values
AMeshGenerator::AMeshGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain Mesh"));
	
}

// Called when the game starts or when spawned
void AMeshGenerator::BeginPlay()
{
	Super::BeginPlay();
	TArray<TArray<float>> map;
	CreateTerrainMesh(map);
}

// Called every frame
void AMeshGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeshGenerator::CreateTerrainMesh(TArray<TArray<float>>& noisemap)
{
	auto location = GetActorLocation();
	
	//int width = noisemap[0].Num() + 1;
	//int height = noisemap.Num() + 1;
	
	TArray<FVector> verteies;
	verteies.Reserve(width * height);

	TArray<int32> triangle_index;
	int indexSize = (width - 1) * (height - 1) * 6;
	triangle_index.Reserve(indexSize);
	

	float scale = 100.f;
	for(int i = 0; i < height; i ++)
	{
		for(int j = 0; j < width; j++)
		{
			float X = (i - (height * 0.5f)) * scale;
			float Y = (j - (width * 0.5f)) * scale;
			verteies.Add(FVector(X, Y, 0) + location);

			if(i < height - 1 && j < width - 1){
				triangle_index.Add(i * width + j);
				triangle_index.Add(i * width + j + 1);
				triangle_index.Add((i + 1) * width + j);
				
				triangle_index.Add(i * width + j + 1);
				triangle_index.Add((i + 1) * width + j + 1);
				triangle_index.Add((i + 1) * width + j);
			}
		}
	}
	
	mesh->CreateMeshSection(0, verteies, triangle_index, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, true);
}

