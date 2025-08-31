// Fill out your copyright notice in the Description page of Project Settings.


#include "LakeSplineGenerator.h"
#include "WaterBodyActor.h"
#include "WaterSplineComponent.h"

using namespace std;

// Sets default values for this component's properties
ULakeSplineGenerator::ULakeSplineGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
}


void ULakeSplineGenerator::Duplicate(TSubclassOf<AActor> actor, FVector pos, FVector rot)
{
	if (actor == nullptr) { return; }

	AActor* newActor = GetWorld()->SpawnActor<AActor>(actor);

	newActor->SetActorLocation(pos + GetOwner()->GetActorLocation() + spawnCenter);
	newActor->SetActorRotation(FRotator(rot.X, rot.Y, rot.Z));


	UE_LOG(LogTemp, Warning, TEXT("new name: %s"), *FString(newActor->GetName()));
	

	screenSpaceDetector->RegisterActor(newActor);
}

template<class T>
T GetRandomElement(TArray<T>& arr)
{
	int arrSize = arr.Num();
	int pickedIndex = FMath::Rand() % arrSize;
	return arr[pickedIndex];
}

template<class T>
T GetWeightedRandomElement(TArray<T>& arr, TArray<float>& weights)
{
	
	double totalWeights = 0;
	for (int i = 0; i < weights.Num(); i++)
	{
		totalWeights += weights[i];
	}

	float pickedWeight = FMath::FRandRange(0, totalWeights);
	
	double currentTotal = 0;
	for (int i = 0; i < weights.Num(); i++)
	{
		currentTotal += weights[i];
		if (currentTotal > pickedWeight)
		{
			return arr[i];
		}
	}
	
	return arr[-1];
}

// Called when the game starts
void ULakeSplineGenerator::BeginPlay()
{
	Super::BeginPlay();

	screenSpaceDetector = screenSpaceActor->GetComponentByClass<UScreenSpaceDetector>();
	UE_LOG(LogTemp, Warning, TEXT("derbis weights: %s, %s"), *FString::SanitizeFloat(lakeDebrisWeights[0]), *FString::SanitizeFloat(lakeDebrisWeights[1]));
	UE_LOG(LogTemp, Warning, TEXT("derbis names: %s, %s"), *FString(lakeDebris[0]->GetName()), *FString(lakeDebris[1]->GetName()));

	for (int i = 0; i < lakeDebrisCount; i++)
	{
		float angle = FMath::FRandRange(0, 2 * PI);
		float r = FMath::FRandRange(minSpawnDistance, maxSpawnDistance);

		FVector spawnPosition(cos(angle)*r, sin(angle)*r, 0);

		FVector rotVector(
			FMath::FRandRange(0, 360.0),
			FMath::FRandRange(0, 360.0),
			FMath::FRandRange(0, 360.0)
		);
		
		auto prefab = GetWeightedRandomElement(lakeDebris, lakeDebrisWeights);

		Duplicate(prefab, spawnPosition, rotVector);
	}
	
}


// Called every frame
void ULakeSplineGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// ...
}

