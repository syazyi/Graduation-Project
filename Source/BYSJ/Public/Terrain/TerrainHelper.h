// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include  "NoiseMapGernerator.h"
#include "TerrainHelper.generated.h"

UCLASS()
class BYSJ_API ATerrainHelper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainHelper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GenerateTerrain();
private:
	UPROPERTY(VisibleAnywhere)
	TArray<ANoiseMapGernerator*> NoiseMapGernerators;
};
