// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "BuildComponent.generated.h"



enum class EBuildType
{
	eBuildFoundation,
	eBuildBoard,
	eBuildLight
};

struct FBuildType
{
	FBuildType() = default;
	UStaticMesh* Mesh;
	ECollisionChannel CollisionChannel;
	TSubclassOf<AActor> Actor;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BYSJ_API UBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuildComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetCamera(UCameraComponent* camera);
	void SpawnBuildPreview();
	void GiveBuildColor(bool isGreen);
	void BuildCycle();
	void BuildDelay();
	void StopBuildMode();
	void LaunchBuildMode();
	void PutBuild();
	void ChangeBuild(EBuildType buildType);
	
protected:
	UPROPERTY(VisibleAnywhere)
	bool m_IsBuildModeOn{false};
	
	UPROPERTY(VisibleAnywhere)
	bool m_CanBuild{false};
	
	int m_BuildID;
	
	UPROPERTY(VisibleAnywhere)
	FTransform m_BuildTransForm;

	UPROPERTY(VisibleAnywhere)
	APawn* m_PlayerRef;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* m_Camera;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_BuildPreview;
private:
	TArray<UMaterialInterface*> m_MaterialsForPreview;
	FTimerHandle m_BuildSystemTimerHandle;
	TMap<EBuildType, FBuildType> m_Builds;
	EBuildType m_CurrentBuild{ EBuildType::eBuildFoundation };
	bool m_CanBeBuild;
	
};


DECLARE_DELEGATE_OneParam(ChangeBuildDelegate, EBuildType)