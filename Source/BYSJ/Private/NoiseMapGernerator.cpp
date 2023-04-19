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
	
	Meshes.Reserve(ParallelPartCount);
	Meshes.Emplace(CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain Mesha")));
	Meshes[0]->SetMobility(EComponentMobility::Static);
	Meshes[0]->SetupAttachment(RootComponent);
	
	for (int i = 1; i < ParallelPartCount; i++)
	{
		char c;
		Int32ToChar(i,&c);
		c += 'a';
		Meshes.Emplace(CreateDefaultSubobject<UProceduralMeshComponent>(FName(FString("Terrain Mesh") + c)));
		Meshes[i]->SetMobility(EComponentMobility::Static);
		Meshes[i]->SetupAttachment(Meshes[0]);
	}
}

// Called when the game starts or when spawned
void ANoiseMapGernerator::BeginPlay()
{
	Super::BeginPlay();
	
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
	noiseMap = NoiseMap::CreateNoiseMap(width, height, scale, GetActorLocation());
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
	auto transfer = GetRootComponent()->GetRelativeTransform();
	auto location = transfer.GetLocation();
	noiseMap = NoiseMap::CreateNoiseMap(width + 1, height + 1, scale, location);
	if(Texture2D == nullptr)
	{
		Texture2D = DrawNoiseMap();
	}
	
	return Texture2D;
}

void ANoiseMapGernerator::CreateTerrainMesh()
{
	
	auto location = GetActorLocation();
	
	int32 MeshesWidth = width;
	int32 MeshesHeight = height;
	
	auto start = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();

	for(int32 chunkIndex = 0; chunkIndex < ParallelPartCount; chunkIndex ++)
	{
		
		auto HeightVertexCount = height; // 4
		auto WidthVertexCount = width + 1; // 5
		auto ChunkHeightCount = HeightVertexCount - 1; // 3
		auto ChunkWidthCount = WidthVertexCount - 1;  // 3
			
		auto ChunkHeight = ChunkHeightCount / ParallelPartCount;// 3 / 3 = 1
		auto ChunkHeightFinal = ChunkHeight;
		if(chunkIndex == ParallelPartCount - 1)
		{
			auto remainder = MeshesHeight % ParallelPartCount;//4
			if(remainder != 0)
			{
				ChunkHeightFinal = ChunkHeightFinal + remainder;//66
			}else
			{
				ChunkHeightFinal = ChunkHeightFinal + ParallelPartCount;
			}
		}
		
		TArray<FVector> verteies;
		verteies.Reserve(WidthVertexCount * (ChunkHeightFinal + 1));//5 * 2
	
		TArray<FVector2D> UV;
		UV.Reserve(WidthVertexCount * (ChunkHeightFinal + 1));//5 * 2

		TArray<int32> triangle_index;
		int indexSize = (WidthVertexCount - 1) * (ChunkHeightFinal) * 6;// 4 * 1 * 6 
		triangle_index.Reserve(indexSize);

		for(int32 index = 0; index < ChunkHeightFinal + 1; index++)
		{
			int32 i = chunkIndex * ChunkHeight + index;// 0 1, 1 2, 2 3
			for(int32 j = 0; j < WidthVertexCount; j++)
			{
				float X = (i - (MeshesHeight * 0.5f)) * MeshScale;
				float Y = (j - (MeshesWidth * 0.5f)) * MeshScale;
				
				verteies.Emplace(FVector(X, Y, (noiseMap[i][j]) * 127.5f * heightOffset));

				UV.Emplace(FVector2D((j + 1.0f) / (WidthVertexCount + 1), (i + 1.0f) / (MeshesHeight + 1)));
					
				if(index < ChunkHeightFinal && j < WidthVertexCount - 1){
					triangle_index.Emplace(index * WidthVertexCount + j);
					triangle_index.Emplace(index * WidthVertexCount + j + 1);
					triangle_index.Emplace((index + 1) * WidthVertexCount + j);
					
					triangle_index.Emplace(index * WidthVertexCount + j + 1);
					triangle_index.Emplace((index + 1) * WidthVertexCount + j + 1);
					triangle_index.Emplace((index + 1) * WidthVertexCount + j);
				}
			}
		}
		Meshes[chunkIndex]->CreateMeshSection(0,verteies, triangle_index, TArray<FVector>{}, UV, TArray<FColor>{}, TArray<FProcMeshTangent>{},
			true);
	}
	
	/*for(int32 chunkIndex = 0; chunkIndex < ParallelPartCount; chunkIndex ++)
	{
		
		auto HeightVertexCount = height; // 4
		auto WidthVertexCount = width + 1; // 5
		auto ChunkHeightCount = HeightVertexCount - 1; // 3
		auto ChunkWidthCount = WidthVertexCount - 1;  // 3
			
		auto ChunkHeight = ChunkHeightCount / ParallelPartCount;// 3 / 3 = 1
		auto ChunkHeightFinal = ChunkHeight;
		if(chunkIndex == ParallelPartCount - 1)
		{
			auto remainder = MeshesHeight % ParallelPartCount;//4
			if(remainder != 0)
			{
				ChunkHeightFinal = ChunkHeightFinal + remainder;//66
			}else
			{
				ChunkHeightFinal = ChunkHeightFinal + ParallelPartCount;
			}
		}
		
		TArray<FVector> verteies;
		verteies.Reserve(WidthVertexCount * (ChunkHeightFinal + 1));//5 * 2
	
		TArray<FVector2D> UV;
		UV.Reserve(WidthVertexCount * (ChunkHeightFinal + 1));//5 * 2

		TArray<int32> triangle_index;
		int indexSize = (WidthVertexCount - 1) * (ChunkHeightFinal) * 6;// 4 * 1 * 6 
		triangle_index.Reserve(indexSize);

		for(int32 index = 0; index < ChunkHeightFinal + 1; index++)
		{
			int32 i = chunkIndex * ChunkHeight + index;// 0 1, 1 2, 2 3
			for(int32 j = 0; j < WidthVertexCount; j++)
			{
				float X = (i - (MeshesHeight * 0.5f)) * MeshScale;
				float Y = (j - (MeshesWidth * 0.5f)) * MeshScale;
				
				verteies.Emplace(FVector(X, Y, (noiseMap[i][j]) * 127.5f * heightOffset));

				UV.Emplace(FVector2D((j + 1.0f) / (WidthVertexCount + 1), (i + 1.0f) / (MeshesHeight + 1)));
					
				if(index < ChunkHeightFinal && j < WidthVertexCount - 1){
					triangle_index.Emplace(index * WidthVertexCount + j);
					triangle_index.Emplace(index * WidthVertexCount + j + 1);
					triangle_index.Emplace((index + 1) * WidthVertexCount + j);
					
					triangle_index.Emplace(index * WidthVertexCount + j + 1);
					triangle_index.Emplace((index + 1) * WidthVertexCount + j + 1);
					triangle_index.Emplace((index + 1) * WidthVertexCount + j);
				}
			}
		}
		Meshes[chunkIndex]->CreateMeshSection(0,verteies, triangle_index, TArray<FVector>{}, UV, TArray<FColor>{}, TArray<FProcMeshTangent>{},
			true);
	}*/
	
	auto end = FDateTime::Now().GetTimeOfDay().GetTotalMilliseconds();
	auto delta = start - end;
	UE_LOG(LogTemp, Warning, TEXT("Mesh Generator spent %f ms"), delta);
}

UTexture2D* ANoiseMapGernerator::DrawNoiseMap()
{
	auto TextureWidth = width;
	auto TextureHeight = height;
	TArray<FColor> color;
	color.SetNum(TextureWidth * TextureHeight);
	
	for(int i = 0; i < TextureHeight; i++){
		for(int j = 0; j < TextureWidth; j++)
		{
			//int index = (i * width + j) * 4;
			int index = i * TextureWidth + j;
			//color[index] = FLinearColor::LerpUsingHSV(FLinearColor::White, FLinearColor::Black, (noiseMap[i][j] + 1) * 0.5f).ToFColor(true);
			color[index] = GetTerrainColor(noiseMap[i][j]);
		}
	}
	
	UTexture2D* texture = UTexture2D::CreateTransient(TextureWidth, TextureHeight);
	void* data = texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(data, color.GetData(), color.Num() * 4);
	texture->PlatformData->Mips[0].BulkData.Unlock();
	texture->UpdateResource();
	
	return texture;
}

void ANoiseMapGernerator::AddTerrains()
{
	FTerrainType DeepSea{ "Deep Sea", -1.0f, -0.7f, FColor(0x0E163F) };
	FTerrainType Water{ "Water", -0.7f, -0.3f, FColor(0x8CCCFF) };
	FTerrainType Sand{ "Sand", -0.3f, 0.0f, FColor(0xE3C78B)};
	FTerrainType Land{ "Land", 0.0f, 0.3f, FColor(0xC3DE80)};
	FTerrainType plateau{"plateau", 0.3f, 0.7f, FColor(0xEEFFD7)};
	FTerrainType Top{"Top", 0.7f, 1.0f, FColor(0xFFFFFF)};
	

	Terrains.Emplace(DeepSea);
	Terrains.Emplace(Water);
	Terrains.Emplace(Sand);
	Terrains.Emplace(Land);
	Terrains.Emplace(plateau);
	Terrains.Emplace(Top);
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
	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(BaseMaterial, this, FName(TEXT("Terrain Material")));
	MaterialInstanceDynamic->SetTextureParameterValue(FName("T2DParam"), Texture2D);

	for(int i = 0; i < ParallelPartCount; i++)
	{
		Meshes[i]->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MaterialInstanceDynamic);
	}
}

void ANoiseMapGernerator::CreateMeshes()
{


}

