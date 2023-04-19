// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Terrain/Terrain.h"
#include "NoiseMapGernerator.generated.h"

struct FTerrainType;

UCLASS()
class BYSJ_API ANoiseMapGernerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoiseMapGernerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GenerateWithCube();

	UFUNCTION(BlueprintCallable)
	UTexture2D* GenerateWithTexture();

	void CreateTerrainMesh();
private:
	UTexture2D* DrawNoiseMap();

	void AddTerrains();

	FColor GetTerrainColor(float height);

	void ApplyMaterial();

	void CreateMeshes();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NoiseSet")
	int width {500};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NoiseSet")
	int height {500};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NoiseSet")
	float scale {4.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseSet")
	float heightOffset { 100.f };

	UPROPERTY(EditAnywhere, Category = "NoiseSet")
	int32 ParallelPartCount{ 25 };

	UPROPERTY(EditAnywhere, Category = "NoiseSet")
	float MeshScale{100.f};
	
	TArray<TArray<float>> noiseMap;
	
	UPROPERTY(VisibleAnywhere)
	UTexture2D* Texture2D;

	UPROPERTY(EditDefaultsOnly)
	TArray<FTerrainType> Terrains;

	UPROPERTY(EditAnywhere)
	TArray<UProceduralMeshComponent*> Meshes;
	
	UMaterial* BaseMaterial;

	
};
