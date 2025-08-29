// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaterBodyActor.h"
#include "ScreenSpaceDetector.h"
#include "Blueprint/UserWidget.h"
#include "LakeSplineGenerator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LAKESIMULATION_API ULakeSplineGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULakeSplineGenerator();

	//UFUNCTION(BlueprintCallable)

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Duplicate(TSubclassOf<AActor> actor, FVector pos, FVector rot);

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> lakeDebris;

	UPROPERTY(EditAnywhere)
	TArray<float> lakeDebrisWeights;
	
	UPROPERTY(EditAnywhere)
	FVector spawnCenter;

	UPROPERTY(EditAnywhere)
	int lakeDebrisCount = 10;


	UPROPERTY(EditAnywhere)
	float minSpawnDistance = 10;

	UPROPERTY(EditAnywhere)
	float maxSpawnDistance = 100;

	UPROPERTY(EditAnywhere)
	AActor* screenSpaceActor;

	UScreenSpaceDetector* screenSpaceDetector;
};
