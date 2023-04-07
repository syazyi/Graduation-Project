// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseMap.h"


NoiseMap::NoiseMap()
{
}

NoiseMap::~NoiseMap()
{
}

TArray<TArray<float>> NoiseMap::CreateNoiseMap(uint32_t width, uint32_t height, float scale)
{
	TArray<TArray<float>> noiseMap;
	TArray<float> temp;
	temp.SetNum(width);
	noiseMap.Init(temp, height);

	if(scale == 0)
	{
		scale = 0.001f;
	}
	
	for(uint32 i = 0; i < height; i++)
	{
		for(uint32 j = 0; j < width; j++)
		{
			FVector2D sample { (static_cast<float>(j) + 1.0f) / width * scale, (static_cast<float>(i) + 1.0f) / height * scale};
			float perlin_value = Perlin(sample);
			//float perlin_value = FMath::PerlinNoise2D(sample);
			//perlin_value = (perlin_value + 1.f) * 127.5;
			noiseMap[i][j] = perlin_value;
		}
	}
	return noiseMap;
}

float NoiseMap::Perlin(FVector2D& input)
{
	FVector2D P_0{ floorf(input.X), floorf(input.Y) };
	FVector2D P_1{P_0.X, P_0.Y + 1.0f};
	FVector2D P_2{P_0.X + 1.0f, P_0.Y};
	FVector2D P_3{P_0.X + 1.0f, P_0.Y + 1.0f};

	FVector2D P_0_Hash = Hash22(P_0);
	FVector2D P_1_Hash = Hash22(P_1);
	FVector2D P_2_Hash = Hash22(P_2);
	FVector2D P_3_Hash = Hash22(P_3);

	FVector2D P_0_Dir = input - P_0;
	FVector2D P_1_Dir = input - P_1;
	FVector2D P_2_Dir = input - P_2;
	FVector2D P_3_Dir = input - P_3;

	float P_0_PerlinValue = FVector2D::DotProduct(P_0_Hash, P_0_Dir);
	float P_1_PerlinValue = FVector2D::DotProduct(P_1_Hash, P_1_Dir);
	float P_2_PerlinValue = FVector2D::DotProduct(P_2_Hash, P_2_Dir);
	float P_3_PerlinValue = FVector2D::DotProduct(P_3_Hash, P_3_Dir);

	float t_x = input.X - P_0.X;
	t_x = EaseCurves(t_x);

	float P_02_PerlinValue = P_0_PerlinValue * (1.f - t_x) + P_2_PerlinValue * t_x;
	float P_13_PerlinValue = P_1_PerlinValue * (1.f - t_x) + P_3_PerlinValue * t_x;

	float t_y = input.Y - P_0.Y;
	t_y = EaseCurves(t_y);

	float output = P_02_PerlinValue * (1.f - t_y) + P_13_PerlinValue * (t_y);
	return output;
}

float NoiseMap::EaseCurves(float x)
{
	return 6.0f * powf(x, 5.0f) - 15.f * powf(x, 4.0f) + 10.f * powf(x, 3.0f);
}

FVector2D NoiseMap::Hash22(FVector2D& input)
{
	FVector2D temp;
	temp.X = FVector2D::DotProduct(input, FVector2D(127.1f, 311.7f));
	temp.Y = FVector2D::DotProduct(input, FVector2D(269.5f, 183.3f));
	
	temp.X = sin(temp.X) * 43758.5453123;
	temp.Y = sin(temp.Y) * 43758.5453123;

	temp.X = (temp.X - floor(temp.X)) * 2.0f - 1.0f;
	temp.Y = (temp.Y - floor(temp.Y)) * 2.0f - 1.0f;

	temp.Normalize();

	return temp;
}
