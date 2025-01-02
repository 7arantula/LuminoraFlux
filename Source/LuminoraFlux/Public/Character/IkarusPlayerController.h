// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "IkarusPlayerController.generated.h"

class AVRBaseControllers;
class AIkarusCharacter;
class UInputMappingContext;
class UInputAction;

UCLASS()
class LUMINORAFLUX_API AIkarusPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void SetupInputComponent() override;

	// VR Player Input Functions
	void Move(const FInputActionValue& Value);
	void SnapTurn(const FInputActionValue& Value);
	void SmoothTurn(const FInputActionValue& Value);
	void TurnMode();
	void SitStand();
	void MenuToggle();
	void TeleportStarted();
	void TeleportTriggering();
	void TeleportCompleted();
	void LeftGrabStarted();
	void LeftGrabCompleted();
	void RightGrabStarted();
	void RightGrabCompleted();

	void EnableControllers();

	//  Hand Interactions Input Functions
	void LeftGripTriggered(const FInputActionValue& Value);
	void LeftGripCompleted();
	
	void RightGripTriggered(const FInputActionValue& Value);
	void RightGripCompleted();
	
	void LeftTriggerTriggered(const FInputActionValue& Value);
	void LeftTriggerCompleted();
	
	void RightTriggerTriggered(const FInputActionValue& Value);
	void RightTriggerCompleted();
	
	void LeftStickTriggered(const FInputActionValue& Value);
	void LeftStickCompleted();
	
	void RightStickTriggered(const FInputActionValue& Value);
	void RightStickCompleted();

	void MenuButtonPressed();
	void MenuButtonReleased();
	
	void LeftPrimaryButtonPressed();
	void LeftPrimaryButtonReleased();
	
	void RightPrimaryButtonPressed();
	void RightPrimaryButtonReleased();

	void LeftSecondaryButtonPressed();
	void LeftSecondaryButtonReleased();
	
	void RightSecondaryButtonPressed();
	void RightSecondaryButtonReleased();

	void IA_LeftTriggerTouched();
	void IA_LeftTriggerUntouched();

	void IA_RightTriggerTouched();
	void IA_RightTriggerUntouched();

	void IA_LeftPrimaryTouched();
	void IA_LeftPrimaryUntouched();

	void IA_RightPrimaryTouched();
	void IA_RightPrimaryUntouched();

	void IA_LeftSecondaryTouched();
	void IA_LeftSecondaryUntouched();

	void IA_RightSecondaryTouched();
	void IA_RightSecondaryUntouched();

	void IA_LeftThumbstickTouched();
	void IA_LeftThumbstickUntouched();

	void IA_RightThumbstickTouched();
	void IA_RightThumbstickUntouched();

protected:

	virtual void BeginPlay() override;
	
private:

	//Components
	UPROPERTY()
	AIkarusCharacter* LuminoraFlux;

	UPROPERTY()
	AVRBaseControllers* LeftController;

	UPROPERTY()
	AVRBaseControllers* RightController;

	/*
	 *  Input Mappings
	 */
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Mappings")
	UInputMappingContext* VRLocomotionMapping;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Mappings")
	UInputMappingContext* PCLocomotionMapping;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Mappings")
	UInputMappingContext* HandInteractionsMapping;

	/**
	 * VR Inputs
	 */
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|VR")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|VR")
	UInputAction* IA_Turn;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|VR")
	UInputAction* IA_SitStand;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|VR")
	UInputAction* IA_MenuToggle;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|VR")
	UInputAction* IA_Teleport;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|VR")
	UInputAction* IA_GrabLeft;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|VR")
	UInputAction* IA_GrabRight;

	/**
	 * PC Inputs
	 */
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|PC")
	UInputAction* IA_MovePC;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|PC")
	UInputAction* IA_LookPC;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|PC")
	UInputAction* IA_SitStandPC;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|PC")
	UInputAction* IA_MenuTogglePC;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|PC")
	UInputAction* IA_TeleportPC;

	/**
	 * Hand Interactions
	 */
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftHandGrip;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightHandGrip;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftHandTrigger;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightHandTrigger;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftHandStick;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightHandStick;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_MenuButton;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftPrimaryButton;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightPrimaryButton;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftSecondaryButton;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightSecondaryButton;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftTriggerTouch;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightTriggerTouch;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftPrimaryTouch;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightPrimaryTouch;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftSecondaryTouch;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightSecondaryTouch;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_LeftThumbstickTouch;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "InputSetup|Inputs|Interactions")
	UInputAction* IA_RightThumbstickTouch;
};