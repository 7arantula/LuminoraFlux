// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMovementComponentAsync.h"
#include "HeadMountedDisplayTypes.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "IkarusCharacter.generated.h"

class AVRBaseControllers;
class UVRGrabComponent;
class UMotionControllerComponent;
class UWidgetInteractionComponent;
class UNiagaraComponent;
class UVRControllerComponent;
class UCameraComponent;

UCLASS()
class LUMINORAFLUX_API AIkarusCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//  Default Inherited Functions
	AIkarusCharacter();
	virtual void Tick(float DeltaTime) override;

	//  Getter Function
	FORCEINLINE UCameraComponent* GetVRCamera() const { return VRCamera; }
	
	//  Player Input Functions
	void Move(const FInputActionValue& Value);
	void SnapTurn(const FInputActionValue& Value);
	void SmoothTurn(const FInputActionValue& Value);
	void SitStand();

	// VR Character Properties
	UPROPERTY(EditDefaultsOnly, Category = "VR Character Properties")
	TEnumAsByte<EHMDTrackingOrigin::Type> TrackingOriginType = EHMDTrackingOrigin::Eye;
	
	UPROPERTY(EditDefaultsOnly, Category = "VR Character Properties")
	bool bIsSnapTurn = true;

	UPROPERTY(EditDefaultsOnly, Category = "VR Character Properties")
	float SnapTurnValue = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "VR Character Properties")
	float TurnRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "VR Character Properties")
	TSubclassOf<AVRBaseControllers> BPLeftController;

	UPROPERTY(EditDefaultsOnly, Category = "VR Character Properties")
	TSubclassOf<AVRBaseControllers> BPRightController;

	UPROPERTY(BlueprintReadOnly)
	AVRBaseControllers* LeftController;

	UPROPERTY(BlueprintReadOnly)
	AVRBaseControllers* RightController;

	UPROPERTY()
	bool bIsInVR = false;

protected:
	
	virtual void BeginPlay() override;

private:
	
	/**
	 * Private Variables
	 */

	UPROPERTY()
	UMaterialInstanceDynamic* FadeMaterialInst;
	
	UPROPERTY()
	float CapsuleHalfHeight;

	UPROPERTY()
	bool bWasFalling;

	UPROPERTY()
	bool bIsSitting;
	
	/**
	 * Components
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* VRCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TeleportFadeSphere;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* VROrigin;

	/**
	 * Private Functions
	 */
	void UpdateCapsuleHalfHeight();
	void UpdateCapsulePosition();

public:

	void SetTeleportFade(float FadeValue);
};