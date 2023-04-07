// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseMapGernerator.h"
#include "NoiseMap.h"
#include "UnitCube.h"

// Sets default values
ANoiseMapGernerator::ANoiseMapGernerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BaseMaterial =  ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/BYSJ/Noise/NoiseMap.NoiseMap'")).Object;
	AddTerrains();
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain Mesh"));
	Mesh->SetMaterial(0, BaseMaterial);
}

// Called when the game starts or when spawned
void ANoiseMapGernerator::BeginPlay()
{
	Super::BeginPlay();

	
	//GenerateWithCube();
	GenerateWithTexture();
	CreateTerrainMesh();
	ApplyMaterial();
}

// Called every frame
void ANoiseMapGernerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANoiseMapGernerator::GenerateWithCube()
{
	noiseMap = NoiseMap::CreateNoiseMap(width, height, scale);
	auto pos = GetActorLocation();
	auto world = GetWorld();
	if(world == nullptr)
	{
		return ;
	}
	for(int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			world->SpawnActor<AActor>(AUnitCube::StaticClass(), pos + FVector(i * 100.f, j * 100.f, noiseMap[i][j] * heightOffset), FRotator());
		}
	}
}

UTexture2D* ANoiseMapGernerator::GenerateWithTexture()
{
	noiseMap = NoiseMap::CreateNoiseMap(width, height, scale);
	if(Texture2D == nullptr)
	{
		Texture2D = DrawNoiseMap();
	}
	
	return Texture2D;
}

void ANoiseMapGernerator::CreateTerrainMesh()
{
	auto location = GetActorLocation();
	
	int MeshesWidth = width;
	int MeshesHeight = height;
	
	TArray<FVector> verteies;
	verteies.Reserve(MeshesWidth * MeshesHeight);

	TArray<int32> triangle_index;
	int indexSize = (MeshesWidth - 1) * (MeshesHeight - 1) * 6;
	triangle_index.Reserve(indexSize);
	
	TArray<FVector2D> UV;
	UV.Reserve(MeshesWidth * MeshesHeight);
	
	float MeshScale = 100.f;
	for(int i = 0; i < MeshesHeight; i ++)
	{
		for(int j = 0; j < MeshesWidth; j++)
		{
			float X = (i - (MeshesHeight * 0.5f)) * MeshScale;
			float Y = (j - (MeshesWidth * 0.5f)) * MeshScale;
			verteies.Add(FVector(X, Y, (noiseMap[i][j] + 1) * 127.5f * heightOffset) + location);

			UV.Add(FVector2D(static_cast<float>(j) / MeshesWidth, static_cast<float>(i) / MeshesHeight));
			if(i < MeshesHeight - 1 && j < MeshesWidth - 1){
				triangle_index.Add(i * MeshesWidth + j);
				triangle_index.Add(i * MeshesWidth + j + 1);
				triangle_index.Add((i + 1) * MeshesWidth + j);
				
				triangle_index.Add(i * MeshesWidth + j + 1);
				triangle_index.Add((i + 1) * MeshesWidth + j + 1);
				triangle_index.Add((i + 1) * MeshesWidth + j);
			}
		}
	}
	
	Mesh->CreateMeshSection(0, verteies, triangle_index, TArray<FVector>{}, UV, TArray<FColor>{}, TArray<FProcMeshTangent>{}, true);
}

UTexture2D* ANoiseMapGernerator::DrawNoiseMap()
{
	
	TArray<FColor> color;
	color.SetNum(width * height);

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++)
		{
			//int index = (i * width + j) * 4;
			int index = i * width + j;
			//color[index] = FLinearColor::LerpUsingHSV(FLinearColor::White, FLinearColor::Black, (noiseMap[i][j] + 1) * 0.5f).ToFColor(true);
			color[index] = GetTerrainColor(noiseMap[i][j]);
		}
	}
	
	UTexture2D* texture = UTexture2D::CreateTransient(width, height);
	void* data = texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(data, color.GetData(), color.Num() * 4);
	texture->PlatformData->Mips[0].BulkData.Unlock();
	texture->UpdateResource();
	
	return texture;
}

void ANoiseMapGernerator::AddTerrains()
{
	FTerrainType water{ "Water", -1.0f, 0.0f, FColor::Blue };
	FTerrainType land{ "land", 0.0f, 0.5f, FColor::Green};
	FTerrainType plateau{"plateau", 0.5f, 1.0f, FColor::White};

	Terrains.Emplace(water);
	Terrains.Emplace(land);
	Terrains.Emplace(plateau);
}

FColor ANoiseMapGernerator::GetTerrainColor(float NoiseMapHeight)
{
	auto size = Terrains.Num();
	for(int i = 0; i < size; i++)
	{
		if(NoiseMapHeight <= Terrains[i].maxHeight && NoiseMapHeight >= Terrains[i].minHeight)
		{
			return Terrains[i].color;
		}
	}
	return FColor::Purple;
}

void ANoiseMapGernerator::ApplyMaterial()
{
	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	MaterialInstanceDynamic->SetTextureParameterValue(FName("T2DParam"), Texture2D);
	
	Mesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MaterialInstanceDynamic);
}

