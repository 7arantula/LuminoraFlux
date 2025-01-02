
// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/IkarusPlayerController.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Character/VRBaseControllers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Character/IkarusCharacter.h"
#include "Character/VRControllerAnimInstance.h"


void AIkarusPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Casting VR Character and Saving it.
	LuminoraFlux = Cast<AIkarusCharacter>(GetPawn());

	FTimerHandle DelayHandle;
	constexpr float DelayTime = 0.2f;
	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AIkarusPlayerController::EnableControllers);
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, TimerDelegate, DelayTime, false);
	
	// Initializing VR Locomotion Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if(UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			Subsystem->AddMappingContext(VRLocomotionMapping, 0);
			Subsystem->AddMappingContext(HandInteractionsMapping, 1);
		}
		else
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(PCLocomotionMapping, 0);
		}
	}
}

void AIkarusPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AIkarusPlayerController::Move);
		EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Started, this, &AIkarusPlayerController::SnapTurn);
		EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AIkarusPlayerController::SmoothTurn);
		EnhancedInputComponent->BindAction(IA_SitStand, ETriggerEvent::Started, this, &AIkarusPlayerController::SitStand);
		EnhancedInputComponent->BindAction(IA_MenuToggle, ETriggerEvent::Started, this, &AIkarusPlayerController::MenuToggle);
		EnhancedInputComponent->BindAction(IA_Teleport, ETriggerEvent::Started, this, &AIkarusPlayerController::TeleportStarted);
		EnhancedInputComponent->BindAction(IA_Teleport, ETriggerEvent::Triggered, this, &AIkarusPlayerController::TeleportTriggering);
		EnhancedInputComponent->BindAction(IA_Teleport, ETriggerEvent::Completed, this, &AIkarusPlayerController::TeleportCompleted);
		EnhancedInputComponent->BindAction(IA_GrabLeft, ETriggerEvent::Started, this, &AIkarusPlayerController::LeftGrabStarted);
		EnhancedInputComponent->BindAction(IA_GrabLeft, ETriggerEvent::Completed, this, &AIkarusPlayerController::LeftGrabCompleted);
		EnhancedInputComponent->BindAction(IA_GrabRight, ETriggerEvent::Started, this, &AIkarusPlayerController::RightGrabStarted);
		EnhancedInputComponent->BindAction(IA_GrabRight, ETriggerEvent::Completed, this, &AIkarusPlayerController::RightGrabCompleted);

		EnhancedInputComponent->BindAction(IA_MovePC, ETriggerEvent::Triggered, this, &AIkarusPlayerController::Move);
		EnhancedInputComponent->BindAction(IA_LookPC, ETriggerEvent::Triggered, this, &AIkarusPlayerController::SmoothTurn);
		EnhancedInputComponent->BindAction(IA_SitStandPC, ETriggerEvent::Started, this, &AIkarusPlayerController::SitStand);
		EnhancedInputComponent->BindAction(IA_MenuTogglePC, ETriggerEvent::Started, this, &AIkarusPlayerController::MenuToggle);
		EnhancedInputComponent->BindAction(IA_TeleportPC, ETriggerEvent::Started, this, &AIkarusPlayerController::TeleportStarted);
		EnhancedInputComponent->BindAction(IA_TeleportPC, ETriggerEvent::Triggered, this, &AIkarusPlayerController::TeleportTriggering);
		EnhancedInputComponent->BindAction(IA_TeleportPC, ETriggerEvent::Completed, this, &AIkarusPlayerController::TeleportCompleted);
		
		EnhancedInputComponent->BindAction(IA_LeftHandGrip, ETriggerEvent::Triggered, this, &AIkarusPlayerController::LeftGripTriggered);
		EnhancedInputComponent->BindAction(IA_LeftHandGrip, ETriggerEvent::Completed, this, &AIkarusPlayerController::LeftGripCompleted);
		EnhancedInputComponent->BindAction(IA_RightHandGrip, ETriggerEvent::Triggered, this, &AIkarusPlayerController::RightGripTriggered);
		EnhancedInputComponent->BindAction(IA_RightHandGrip, ETriggerEvent::Completed, this, &AIkarusPlayerController::RightGripCompleted);
		EnhancedInputComponent->BindAction(IA_LeftHandTrigger, ETriggerEvent::Triggered, this, &AIkarusPlayerController::LeftTriggerTriggered);
		EnhancedInputComponent->BindAction(IA_LeftHandTrigger, ETriggerEvent::Completed, this, &AIkarusPlayerController::LeftTriggerCompleted);
		EnhancedInputComponent->BindAction(IA_RightHandTrigger, ETriggerEvent::Triggered, this, &AIkarusPlayerController::RightTriggerTriggered);
		EnhancedInputComponent->BindAction(IA_RightHandTrigger, ETriggerEvent::Completed, this, &AIkarusPlayerController::RightTriggerCompleted);
		EnhancedInputComponent->BindAction(IA_LeftHandStick, ETriggerEvent::Triggered, this, &AIkarusPlayerController::LeftStickTriggered);
		EnhancedInputComponent->BindAction(IA_LeftHandStick, ETriggerEvent::Completed, this, &AIkarusPlayerController::LeftStickCompleted);
		EnhancedInputComponent->BindAction(IA_RightHandStick, ETriggerEvent::Triggered, this, &AIkarusPlayerController::RightStickTriggered);
		EnhancedInputComponent->BindAction(IA_RightHandStick, ETriggerEvent::Completed, this, &AIkarusPlayerController::RightStickCompleted);
		EnhancedInputComponent->BindAction(IA_MenuButton, ETriggerEvent::Triggered, this, &AIkarusPlayerController::MenuButtonPressed);
		EnhancedInputComponent->BindAction(IA_MenuButton, ETriggerEvent::Completed, this, &AIkarusPlayerController::MenuButtonReleased);
		EnhancedInputComponent->BindAction(IA_LeftPrimaryButton, ETriggerEvent::Triggered, this, &AIkarusPlayerController::LeftPrimaryButtonPressed);
		EnhancedInputComponent->BindAction(IA_LeftPrimaryButton, ETriggerEvent::Completed, this, &AIkarusPlayerController::LeftPrimaryButtonReleased);
		EnhancedInputComponent->BindAction(IA_RightPrimaryButton, ETriggerEvent::Triggered, this, &AIkarusPlayerController::RightPrimaryButtonPressed);
		EnhancedInputComponent->BindAction(IA_RightPrimaryButton, ETriggerEvent::Completed, this, &AIkarusPlayerController::RightPrimaryButtonReleased);
		EnhancedInputComponent->BindAction(IA_LeftSecondaryButton, ETriggerEvent::Triggered, this, &AIkarusPlayerController::LeftSecondaryButtonPressed);
		EnhancedInputComponent->BindAction(IA_LeftSecondaryButton, ETriggerEvent::Completed, this, &AIkarusPlayerController::LeftSecondaryButtonReleased);
		EnhancedInputComponent->BindAction(IA_RightSecondaryButton, ETriggerEvent::Triggered, this, &AIkarusPlayerController::RightSecondaryButtonPressed);
		EnhancedInputComponent->BindAction(IA_RightSecondaryButton, ETriggerEvent::Completed, this, &AIkarusPlayerController::RightSecondaryButtonReleased);
		EnhancedInputComponent->BindAction(IA_LeftTriggerTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_LeftTriggerTouched);
		EnhancedInputComponent->BindAction(IA_LeftTriggerTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_LeftTriggerUntouched);
		EnhancedInputComponent->BindAction(IA_RightTriggerTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_RightTriggerTouched);
		EnhancedInputComponent->BindAction(IA_RightTriggerTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_RightTriggerUntouched);
		EnhancedInputComponent->BindAction(IA_LeftPrimaryTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_LeftPrimaryTouched);
		EnhancedInputComponent->BindAction(IA_LeftPrimaryTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_LeftPrimaryUntouched);
		EnhancedInputComponent->BindAction(IA_RightPrimaryTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_RightPrimaryTouched);
		EnhancedInputComponent->BindAction(IA_RightPrimaryTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_RightPrimaryUntouched);
		EnhancedInputComponent->BindAction(IA_LeftSecondaryTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_LeftSecondaryTouched);
		EnhancedInputComponent->BindAction(IA_LeftSecondaryTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_LeftSecondaryUntouched);
		EnhancedInputComponent->BindAction(IA_RightSecondaryTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_RightSecondaryTouched);
		EnhancedInputComponent->BindAction(IA_RightSecondaryTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_RightSecondaryUntouched);
		EnhancedInputComponent->BindAction(IA_LeftThumbstickTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_LeftThumbstickTouched);
		EnhancedInputComponent->BindAction(IA_LeftThumbstickTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_LeftThumbstickUntouched);
		EnhancedInputComponent->BindAction(IA_RightThumbstickTouch, ETriggerEvent::Started, this, &AIkarusPlayerController::IA_RightThumbstickTouched);
		EnhancedInputComponent->BindAction(IA_RightThumbstickTouch, ETriggerEvent::Completed, this, &AIkarusPlayerController::IA_RightThumbstickUntouched);
	}
}

void AIkarusPlayerController::EnableControllers()
{
	if(LuminoraFlux)
	{
		LeftController = Cast<AVRBaseControllers>(LuminoraFlux->LeftController);
		RightController = Cast<AVRBaseControllers>(LuminoraFlux->RightController);
	}
}

//  VR Player Input Functions
void AIkarusPlayerController::Move(const FInputActionValue& Value)
{
	//  Calling the Move Function in LuminoraFlux
	if(LuminoraFlux)
	{
		LuminoraFlux->Move(Value);
	}
}
void AIkarusPlayerController::SnapTurn(const FInputActionValue& Value)
{
	//  Calling the Look Function in LuminoraFlux
	if(LuminoraFlux)
	{
		LuminoraFlux->SnapTurn(Value);
	}
}
void AIkarusPlayerController::SmoothTurn(const FInputActionValue& Value)
{
	//  Calling the Look Function in LuminoraFlux
	if(LuminoraFlux)
	{
		LuminoraFlux->SmoothTurn(Value);
	}
}
void AIkarusPlayerController::TurnMode()
{
	//  Turn Mode in LuminoraFlux
	if(LuminoraFlux)
	{
		if(LuminoraFlux->bIsSnapTurn)
		{
			LuminoraFlux->bIsSnapTurn = false;
		}
		else
		{
			LuminoraFlux->bIsSnapTurn = true;
		}
	}
}
void AIkarusPlayerController::SitStand()
{
	if(LuminoraFlux)
	{
		LuminoraFlux->SitStand();
	}
}
void AIkarusPlayerController::MenuToggle()
{
	// Toggle Menu in Controllers
	if(LeftController)
	{
		LeftController->MenuToggle();
	}
}
void AIkarusPlayerController::TeleportStarted()
{
	// Start Teleport in VR Character
	if(LeftController)
	{
		LeftController->TeleportStarted();
	}
}
void AIkarusPlayerController::TeleportTriggering()
{
	// Trigger Teleport in VR Character
	if(LeftController)
	{
		LeftController->TeleportTriggering();
	}
}
void AIkarusPlayerController::TeleportCompleted()
{
	// Complete Trigger in VR Character
	if(LeftController)
	{
		LeftController->TeleportCompleted();
	}
}
void AIkarusPlayerController::LeftGrabStarted()
{
	if(LeftController)
	{
		LeftController->LeftGrabStarted();
	}
}
void AIkarusPlayerController::LeftGrabCompleted()
{
	if(LeftController)
	{
		LeftController->LeftGrabCompleted();
	}
}
void AIkarusPlayerController::RightGrabStarted()
{
	if(RightController)
	{
		RightController->RightGrabStarted();
	}
}
void AIkarusPlayerController::RightGrabCompleted()
{
	if(RightController)
	{
		RightController->RightGrabCompleted();
	}
}

//  Hand Interactions Input Functions
void AIkarusPlayerController::LeftGripTriggered(const FInputActionValue& Value)
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		const float InputValue = Value.Get<float>();
		LeftController->ControllerAnimInstance->LeftGripAlpha = InputValue;
	}
}
void AIkarusPlayerController::LeftGripCompleted()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftGripAlpha = 0.f;
	}
}

void AIkarusPlayerController::RightGripTriggered(const FInputActionValue& Value)
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		const float InputValue = Value.Get<float>();
		RightController->ControllerAnimInstance->RightGripAlpha = InputValue;
	}
}
void AIkarusPlayerController::RightGripCompleted()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightGripAlpha = 0.f;
	}
}

void AIkarusPlayerController::LeftTriggerTriggered(const FInputActionValue& Value)
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		const float InputValue = Value.Get<float>();
		LeftController->ControllerAnimInstance->LeftTriggerAlpha = InputValue;
	}
}
void AIkarusPlayerController::LeftTriggerCompleted()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftTriggerAlpha = 0.f;
	}
}

void AIkarusPlayerController::RightTriggerTriggered(const FInputActionValue& Value)
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		const float InputValue = Value.Get<float>();
		RightController->ControllerAnimInstance->RightTriggerAlpha = InputValue;
	}
}
void AIkarusPlayerController::RightTriggerCompleted()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightTriggerAlpha = 0.f;
	}
}

void AIkarusPlayerController::LeftStickTriggered(const FInputActionValue& Value)
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		const FVector InputValue = Value.Get<FVector>();
		LeftController->ControllerAnimInstance->LeftStickAlpha = InputValue;
	}
}
void AIkarusPlayerController::LeftStickCompleted()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftStickAlpha = FVector::ZeroVector;
	}
}

void AIkarusPlayerController::RightStickTriggered(const FInputActionValue& Value)
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		const FVector InputValue = Value.Get<FVector>();
		RightController->ControllerAnimInstance->RightStickAlpha = InputValue;
	}
}
void AIkarusPlayerController::RightStickCompleted()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightStickAlpha = FVector::ZeroVector;
	}
}

void AIkarusPlayerController::MenuButtonPressed()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftMenuButtonPressed = true;
	}
}
void AIkarusPlayerController::MenuButtonReleased()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftMenuButtonPressed = false;
	}
}

void AIkarusPlayerController::LeftPrimaryButtonPressed()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftPrimaryButtonPressed = true;
	}
}
void AIkarusPlayerController::LeftPrimaryButtonReleased()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftPrimaryButtonPressed = false;
	}
}

void AIkarusPlayerController::RightPrimaryButtonPressed()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightPrimaryButtonPressed = true;
	}
}
void AIkarusPlayerController::RightPrimaryButtonReleased()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightPrimaryButtonPressed = false;
	}
}

void AIkarusPlayerController::LeftSecondaryButtonPressed()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftSecondaryButtonPressed = true;
	}
}
void AIkarusPlayerController::LeftSecondaryButtonReleased()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftSecondaryButtonPressed = false;
	}
}

void AIkarusPlayerController::RightSecondaryButtonPressed()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightSecondaryButtonPressed = true;
	}
}
void AIkarusPlayerController::RightSecondaryButtonReleased()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightSecondaryButtonPressed = false;
	}
}

void AIkarusPlayerController::IA_LeftTriggerTouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftTriggerTouch = true;
	}
}
void AIkarusPlayerController::IA_LeftTriggerUntouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftTriggerTouch = false;
	}
}

void AIkarusPlayerController::IA_RightTriggerTouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightTriggerTouch = true;
	}
}
void AIkarusPlayerController::IA_RightTriggerUntouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightTriggerTouch = false;
	}
}

void AIkarusPlayerController::IA_LeftPrimaryTouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftPrimaryTouch = true;
	}
}
void AIkarusPlayerController::IA_LeftPrimaryUntouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftPrimaryTouch = false;
	}
}

void AIkarusPlayerController::IA_RightPrimaryTouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightPrimaryTouch = true;
	}
}
void AIkarusPlayerController::IA_RightPrimaryUntouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightPrimaryTouch = false;
	}
}

void AIkarusPlayerController::IA_LeftSecondaryTouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftSecondaryTouch = true;
	}
}
void AIkarusPlayerController::IA_LeftSecondaryUntouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftSecondaryTouch = false;
	}
}

void AIkarusPlayerController::IA_RightSecondaryTouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightSecondaryTouch = true;
	}
}
void AIkarusPlayerController::IA_RightSecondaryUntouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightSecondaryTouch = false;
	}
}

void AIkarusPlayerController::IA_LeftThumbstickTouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftThumbstickTouch = true;
	}
}
void AIkarusPlayerController::IA_LeftThumbstickUntouched()
{
	if(LeftController && LeftController->ControllerAnimInstance)
	{
		LeftController->ControllerAnimInstance->LeftThumbstickTouch = false;
	}
}

void AIkarusPlayerController::IA_RightThumbstickTouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightThumbstickTouch = true;
	}
}
void AIkarusPlayerController::IA_RightThumbstickUntouched()
{
	if(RightController && RightController->ControllerAnimInstance)
	{
		RightController->ControllerAnimInstance->RightThumbstickTouch = false;
	}
}