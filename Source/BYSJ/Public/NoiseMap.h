// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BYSJ_API NoiseMap
{
public:
	NoiseMap();
	~NoiseMap();
	
	static TArray<TArray<float>> CreateNoiseMap(uint32_t width, uint32_t height, float scale, FVector Location);
private:
	static FVector2D Hash22(FVector2D& input);

	static float Perlin(FVector2D& input);

	static float EaseCurves(float x);
	
};
