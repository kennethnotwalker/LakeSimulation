// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <utility>
#include "Blueprint/UserWidget.h"
#include "RectDisplayWidget.generated.h"


/**
 * 
 */
UCLASS()
class LAKESIMULATION_API URectDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

	public:
		virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

		void AddToQueue(std::pair<FVector2D, FVector2D> center);
		void ResetQueue();
		TArray<std::pair<FVector2D, FVector2D>> boxQueue;
};
