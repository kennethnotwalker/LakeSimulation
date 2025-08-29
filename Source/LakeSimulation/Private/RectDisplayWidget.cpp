// Fill out your copyright notice in the Description page of Project Settings.


#include "RectDisplayWidget.h"
#include <utility>
#include "Blueprint/WidgetBlueprintLibrary.h"


void URectDisplayWidget::AddToQueue(std::pair<FVector2D, FVector2D> center)
{
	boxQueue.Add(center);
}

void URectDisplayWidget::ResetQueue()
{
	boxQueue.Empty();
}

int32 URectDisplayWidget::NativePaint
(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
) const
{
	Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext Context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	for (int i = 0; i < boxQueue.Num(); i++)
	{
		std::pair<FVector2D, FVector2D> points = boxQueue[i];

		FVector2D topRight = Context.AllottedGeometry.GetAbsolutePositionAtCoordinates(points.second);
		FVector2D bottomLeft = Context.AllottedGeometry.GetAbsolutePositionAtCoordinates(points.first);
		FVector2D topLeft = FVector2D(bottomLeft.X, topRight.Y);
		FVector2D bottomRight = FVector2D(topRight.X, bottomLeft.Y);

		//convert absolute to local
		
		topRight = Context.AllottedGeometry.AbsoluteToLocal(topRight);
		topLeft = Context.AllottedGeometry.AbsoluteToLocal(topLeft);
		bottomRight = Context.AllottedGeometry.AbsoluteToLocal(bottomRight);
		bottomLeft = Context.AllottedGeometry.AbsoluteToLocal(bottomLeft);

		UWidgetBlueprintLibrary::DrawLine(Context, topRight, topLeft, FLinearColor::White, true, 0.5f);
		UWidgetBlueprintLibrary::DrawLine(Context, topLeft, bottomLeft, FLinearColor::White, true, 0.5f);
		UWidgetBlueprintLibrary::DrawLine(Context, bottomLeft, bottomRight, FLinearColor::White, true, 0.5f);
		UWidgetBlueprintLibrary::DrawLine(Context, bottomRight, topRight, FLinearColor::White, true, 0.5f);
	}

	return LayerId;
}