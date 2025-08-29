// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraActor.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "RectContainer.h"
#include "vector"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/DirectionalLightComponent.h"
#include "ScreenSpaceDetector.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LAKESIMULATION_API UScreenSpaceDetector : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UScreenSpaceDetector();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RegisterActor(AActor* actor);
	TArray<AActor*> registeredActors;
	std::vector<FString> names = {FString("Log"), FString("Rock")};

	void WriteToYOLOv8(TArray<RectContainer> featureList, int suffix);

	void WriteRenderTargetToDisk(UTextureRenderTarget2D* target2D, int suffix);

	void AlignSkyToTime(double skyTime);

	FString GetSharedFileDirectory(int suffix, FString fileType, int mode);

	FString GetSharedFolder(int mode);

	void VerifyTargetDirectory();

	std::pair<FVector2D, FVector2D> GetActorScreenSpace(AActor* actor);
	int frameCounter = 0;
	int saveMode = 0;
	
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* screenshotTexture;
	
	
	UDirectionalLightComponent* skyLight;
	UDirectionalLightComponent* moonLight;

	UPROPERTY(EditAnywhere)
	AActor* skyboxActor;

	UPROPERTY(EditAnywhere)
	AActor* boatActor;

	UPROPERTY(EditAnywhere)
	ACameraActor* cameraActor;

	UPROPERTY(EditAnywhere)
	AActor* captureActor;

	UPROPERTY(EditAnywhere)
	AActor* lake;

	UPROPERTY(EditAnywhere)
	double worldTime = 3600*12;

	UPROPERTY(EditAnywhere)
	double orbitRadius = 5000;

	UPROPERTY(EditAnywhere)
	double minOrbitRadius = 250;

	UPROPERTY(EditAnywhere)
	bool randomizePosition = false;

	UPROPERTY(EditAnywhere)
	bool enableCapture = false;

	USceneCaptureComponent2D* captureComponent;

	UPROPERTY(EditAnywhere)
	int framesPerCameraAngle = 500;

	UPROPERTY(EditAnywhere)
	float targetZ = 500.0;

	UPROPERTY(EditAnywhere)
	FString targetDataFolder;

	UPROPERTY()
	UUserWidget* widget;


	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> WidgetReference;
		
};
