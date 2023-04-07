// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MeshGenerator.generated.h"

UCLASS()
class BYSJ_API AMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateTerrainMesh(TArray<TArray<float>>& noisemap);
protected:
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* mesh;

	UPROPERTY(EditAnywhere)
	int width;
	
	UPROPERTY(EditAnywhere)
	int height;
};
