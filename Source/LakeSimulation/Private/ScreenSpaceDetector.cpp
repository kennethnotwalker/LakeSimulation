// Fill out your copyright notice in the Description page of Project Settings.

#include "ScreenSpaceDetector.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "RectDisplayWidget.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include <vector>
#include "RectContainer.h"
#include "UnrealClient.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Runtime/ImageWriteQueue/Public/ImageWriteBlueprintLibrary.h"
#include "HAL/PlatformFileManager.h"
#include "Components/DirectionalLightComponent.h"

// Sets default values for this component's properties
UScreenSpaceDetector::UScreenSpaceDetector()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UScreenSpaceDetector::BeginPlay()
{
	Super::BeginPlay();
	// ...
	Scalability::FQualityLevels quality;

	quality.SetFromSingleQualityLevel(0);

	

	Scalability::SetQualityLevels(quality);

	int vX, vY;

	GetWorld()->GetFirstPlayerController()->GetViewportSize(vX, vY);

	if (IsValid(WidgetReference))
	{
		widget = CreateWidget(GetWorld(), WidgetReference);
	}
	if (IsValid(widget))
	{
		widget->AddToViewport();
	}

	captureComponent = captureActor->GetComponentByClass<USceneCaptureComponent2D>();
	skyLight = skyboxActor->FindComponentByTag<UDirectionalLightComponent>("Basic Light");
	moonLight = skyboxActor->FindComponentByTag<UDirectionalLightComponent>("Moon Light");

	//screenshotTexture->ResizeTarget(vX, vY);
	//captureComponent->TextureTarget = screenshotTexture;
	
}

void UScreenSpaceDetector::AlignSkyToTime(double skyTime)
{
	double maxTime = 3600 * 24;
	double maxRotation = 360;

	double rotation = (skyTime / maxTime) * maxRotation - 90;

	double moonRotation = rotation + 180;
	if (moonRotation > 360) { moonRotation -= 360; }

	if (-moonRotation <= 0 && -moonRotation >= -180)
	{
		double old = moonRotation;
		double center = 90;
		double dist = moonRotation - center;
		dist *= 0.75;
		moonRotation = center + dist;

		UE_LOG(LogTemp, Warning, TEXT("%s -> %s"), *FString::SanitizeFloat(old), *FString::SanitizeFloat(moonRotation));
	}

	FVector rotVector = FVector(180, -rotation, -45);
	FVector moon_rotVector = FVector(180, -moonRotation, -45);

	FRotator rot = FRotator::MakeFromEuler(rotVector);
	FRotator moonRot = FRotator::MakeFromEuler(moon_rotVector);

	skyLight->SetWorldRotation(rot);
	moonLight->SetWorldRotation(moonRot);
}

FVector2D GetResolution()
{
	FVector2D result(GSystemResolution.ResX, GSystemResolution.ResY);

	return result;
}

FString UScreenSpaceDetector::GetSharedFolder(int mode)
{
	FString filePath;

	if (mode == 0)
	{
		filePath = FString("");
		filePath.Append(targetDataFolder);
	}
	else
	{
		filePath = FString("ValidatedData");
	}

	return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), filePath);;
}

FString UScreenSpaceDetector::GetSharedFileDirectory(int suffix, FString fileType, int mode)
{
	FString filePath;

	if (mode == 0)
	{
		filePath = FString("");
		filePath.Append(targetDataFolder);
		filePath.Append("/data_");
	}
	else
	{
		filePath = FString("ValidatedData/data_");
	}

	filePath.AppendInt(suffix);
	filePath.Append(".");
	filePath.Append(fileType);

	return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), filePath);
}


void UScreenSpaceDetector::VerifyTargetDirectory()
{
	
	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();

	FString path = GetSharedFolder(saveMode);

	if (!FPaths::DirectoryExists(path))
	{
		platformFile.CreateDirectoryTree(*path);
	}

	UE_LOG(LogTemp, Warning, TEXT("verified %s"), *path);
}

void UScreenSpaceDetector::WriteToYOLOv8(TArray<RectContainer> featureList, int suffix)
{
	FString data;
	
	for (int i = 0; i < featureList.Num(); i++)
	{
		RectContainer rectContainer = featureList[i];

		int id = std::find(names.begin(), names.end(), rectContainer.label) - names.begin();

		double centerX = 0.5 * (rectContainer.rect.second.X + rectContainer.rect.first.X);
		double centerY = 0.5 * (rectContainer.rect.second.Y + rectContainer.rect.first.Y);
		double width = (rectContainer.rect.second.X - rectContainer.rect.first.X);
		double height = (rectContainer.rect.second.Y - rectContainer.rect.first.Y);

		data.Append(FString::FromInt(id)); data.Append(" ");
		data.Append(FString::SanitizeFloat(centerX)); data.Append(" ");
		data.Append(FString::SanitizeFloat(centerY)); data.Append(" ");
		data.Append(FString::SanitizeFloat(width)); data.Append(" ");
		data.Append(FString::SanitizeFloat(height)); data.Append("\n");
	}
	

	FFileHelper::SaveStringToFile(data, *GetSharedFileDirectory(suffix, "txt", saveMode));
	
}

void UScreenSpaceDetector::WriteRenderTargetToDisk(UTextureRenderTarget2D* target2D, int suffix)
{
	FImageWriteOptions imageWriteOptions;
	imageWriteOptions.CompressionQuality = 100;
	imageWriteOptions.bOverwriteFile = true;
	imageWriteOptions.bAsync = true;
	imageWriteOptions.Format = EDesiredImageFormat::JPG;
	UImageWriteBlueprintLibrary::ExportToDisk(target2D, *GetSharedFileDirectory(suffix, "jpg", saveMode), imageWriteOptions);
}

bool inRange(float x, float low, float high)
{
	return x >= low && x <= high;
}

bool inRange(FVector2D A, float low, float high)
{
	return inRange(A.X, low, high) && inRange(A.Y, low, high);
}

// Called every frame
void UScreenSpaceDetector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	worldTime += DeltaTime*60;

	if (worldTime >= 3600 * 24)
	{
		worldTime -= 3600 * 24;
	}

	frameCounter++;

	if (IsValid(widget))
	{
		((URectDisplayWidget*)widget)->ResetQueue();
	}
	

	TArray<RectContainer> rects;

	if (enableCapture)
	{
		for (int i = 0; i < registeredActors.Num(); i++)
		{
			std::pair<FVector2D, FVector2D> sPos = GetActorScreenSpace(registeredActors[i]);

			bool empty = sPos.first.SquaredLength() == 0 && sPos.second.SquaredLength() == 0;

			if (inRange(sPos.first, 0, 1) && inRange(sPos.second, 0, 1) && !empty) {

				FString flabel;
				FString dummy;

				registeredActors[i]->GetName().Split("_C", &flabel, &dummy);

				UE_LOG(LogTemp, Warning, TEXT("reg name: '%s' -> '%s'"), *FString(registeredActors[i]->GetName()), *flabel);

				RectContainer rect(flabel, sPos);

				rects.Add(rect);

				if (IsValid(widget))
				{
					((URectDisplayWidget*)widget)->AddToQueue(sPos);
				}

			}
		}

		if (frameCounter > 5)
		{
			VerifyTargetDirectory();
			WriteToYOLOv8(rects, frameCounter);
			WriteRenderTargetToDisk(screenshotTexture, frameCounter);
		}
	}
	

	
	

	if (frameCounter > 5 && frameCounter%framesPerCameraAngle == 0 && randomizePosition)
	{
		UE_LOG(LogTemp, Warning, TEXT("Frame: %s"), *FString::FromInt(frameCounter % 10));

		UE_LOG(LogTemp, Warning, TEXT("Frame: Switching Cam!"));

		double pitch = 0;// FMath::FRandRange(PI / 24, PI / 2);
		double yaw = FMath::FRandRange(0, 2 * PI);

		FVector orbitDirection(cos(yaw) * cos(pitch), sin(yaw) * cos(pitch), sin(pitch));

		int originIndex = FMath::RandRange(0, registeredActors.Num()-1);

		FVector origin = registeredActors[originIndex]->GetActorLocation();

		

		FVector loc = origin + FMath::FRandRange(minOrbitRadius, orbitRadius) * orbitDirection;

		loc.Z = boatActor->GetActorLocation().Z;

		FVector lookDirection = (lake->GetActorLocation() - loc);

		lookDirection.Z = 0; //make sure the boat does not look up or down

		lookDirection.Normalize();

		FRotator directionRotation(lookDirection.Rotation());

		boatActor->SetActorLocation(loc);
		boatActor->SetActorRotation(directionRotation);

		if (targetDataFolder.Equals(FString("WrittenData")))
		{
			saveMode = 1 - saveMode;
		}
	}

	//randomize time
	if (frameCounter > 5 && frameCounter % (framesPerCameraAngle * 2) == 0 && randomizePosition)
	{
		double times[] = {3600 * 6, 3600 * 12, 3600 * 19.5};
		worldTime = times[FMath::RandRange(0, 2)];
	}
	
	//set skybox to match time
	AlignSkyToTime(worldTime);
	
	// ...
}

void UScreenSpaceDetector::RegisterActor(AActor* actor)
{
	registeredActors.Add(actor);
}



std::pair<FVector2D, FVector2D> UScreenSpaceDetector::GetActorScreenSpace(AActor* actor)
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();

	FVector2D screenMin, screenMax;

	UStaticMeshComponent* umesh = actor->GetComponentByClass<UStaticMeshComponent>();
	FBox bounds = umesh->Bounds.GetBox();
	
	FVector verts[8];

	bounds.GetVertices(verts);

	playerController->ProjectWorldLocationToScreen(bounds.Min, screenMin);
	playerController->ProjectWorldLocationToScreen(bounds.Max, screenMax);

	for (int i = 0; i < 8; i++)
	{
		FVector2D point;
		playerController->ProjectWorldLocationToScreen(verts[i], point);

		screenMin.X = fmin(screenMin.X, point.X);
		screenMin.Y = fmin(screenMin.Y, point.Y);
		screenMax.X = fmax(screenMax.X, point.X);
		screenMax.Y = fmax(screenMax.Y, point.Y);
	}

	int vX, vY;

	GetWorld()->GetFirstPlayerController()->GetViewportSize(vX, vY);


	FVector2D normalizedScreenMin(screenMin.X / (float)vX, screenMin.Y / (float)vY);
	FVector2D normalizedScreenMax(screenMax.X / (float)vX, screenMax.Y / (float)vY);

	return std::pair<FVector2D, FVector2D>(normalizedScreenMin, normalizedScreenMax);
}

