// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class LAKESIMULATION_API RectContainer
{
public:
	RectContainer(FString _label, std::pair<FVector2D, FVector2D>& _rect);
	~RectContainer();
	FString label;
	std::pair<FVector2D, FVector2D> rect;
};
