// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "BaseAllEnum.generated.h"

/**
 * 
 */
UENUM()
enum class EPoseType : uint8
{
	EPT_Nothing UMETA(DisplayName = "Nothing"),
	EPT_Knob UMETA(DisplayName = "Knob"),
	EPT_FlatPalm UMETA(DisplayName = "FlatPalm"),
	EPT_ThumbHold UMETA(DisplayName = "ThumbHold"),
	EPT_AllFingureHold UMETA(DisplayName = "AllFingureHold")
};

UCLASS()
class LUMINORAFLUX_API UBaseAllEnum : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
