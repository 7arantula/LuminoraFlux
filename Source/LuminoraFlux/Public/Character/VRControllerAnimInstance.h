// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAllEnum.h"
#include "VRControllerAnimInstance.generated.h"



UCLASS()
class LUMINORAFLUX_API UVRControllerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab Parameters")
	EPoseType PoseType;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Left")
	float LeftGripAlpha = 0.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Right")
	float RightGripAlpha = 0.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Left")
	float LeftTriggerAlpha = 0.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Right")
	float RightTriggerAlpha = 0.f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Left")
	FVector LeftStickAlpha;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Right")
	FVector RightStickAlpha;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Left")
	bool LeftMenuButtonPressed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Left")
	bool LeftPrimaryButtonPressed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Right")
	bool RightPrimaryButtonPressed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Left")
	bool LeftSecondaryButtonPressed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ButtonInputs|Right")
	bool RightSecondaryButtonPressed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Left")
	bool LeftTriggerTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Right")
	bool RightTriggerTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Left")
	bool LeftPrimaryTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Right")
	bool RightPrimaryTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Left")
	bool LeftSecondaryTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Right")
	bool RightSecondaryTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Left")
	bool LeftThumbstickTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TouchInputs|Right")
	bool RightThumbstickTouch;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	bool LeftGrabbed;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	bool RightGrabbed;

	
	
};
