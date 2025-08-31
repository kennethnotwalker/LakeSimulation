// Fill out your copyright notice in the Description page of Project Settings.


#include "RectContainer.h"

RectContainer::RectContainer(FString _label, std::pair<FVector2D, FVector2D>& _rect)
{
	label = _label;
	rect = _rect;
}

RectContainer::~RectContainer()
{
}
