// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/IkarusCharacter.h"

#include "HeadMountedDisplayFunctionLibrary.h"

#include "Camera/CameraComponent.h"
#include "Character/VRBaseControllers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AIkarusCharacter::AIkarusCharacter()
{
	//  Construct Default Components
	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR Origin"));
	VROrigin->SetupAttachment(GetRootComponent());
	
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VROrigin);

	TeleportFadeSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportFadeSphere"));
	TeleportFadeSphere->SetupAttachment(GetVRCamera());
	TeleportFadeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//  Removing Some Default Components
	GetMesh()->DestroyComponent();
	GetMesh()->SetActive(false);

	//  Set some Defaults
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCapsuleHalfHeight(10.f);
	GetCapsuleComponent()->SetCapsuleRadius(10.f);
	
}

void AIkarusCharacter::BeginPlay()
{
	Super::BeginPlay();

	//  Spawn Controllers
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);

	if(BPLeftController)
	{
		LeftController = GetWorld()->SpawnActor<AVRBaseControllers>(BPLeftController, SpawnParams);
		if(LeftController)
		{
			LeftController->AttachToComponent(VROrigin, AttachmentRules);
		}
	}
	if(BPRightController)
	{
		RightController = GetWorld()->SpawnActor<AVRBaseControllers>(BPRightController, SpawnParams);
		if(RightController)
		{
			RightController->AttachToComponent(VROrigin, AttachmentRules);
		}
	}

	//Setup Teleport FadeMaterial
	UMaterialInterface* FadeMaterial =  TeleportFadeSphere->GetMaterial(0);
	FadeMaterialInst =  UMaterialInstanceDynamic::Create(FadeMaterial, this);
	
	
	if(UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(TrackingOriginType);
		bIsInVR = true;
	}
	
	if(!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		//VRCamera->bUsePawnControlRotation = true;
		bIsSnapTurn = false;
		bIsInVR = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		VRCamera->bUsePawnControlRotation = true;
		VRCamera->AddWorldOffset(FVector(0.f, 0.f, 150.f - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	}

	//  Initializing Some Variables
	CapsuleHalfHeight =  GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
}

void AIkarusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCapsuleHalfHeight();
	UpdateCapsulePosition();
}

/**
 * Player Inputs
 */

void AIkarusCharacter::Move(const FInputActionValue& Value)
{
	//  Move Character
	if(Controller)
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();

		const FRotator Rotation = Controller->GetControlRotation() + VRCamera->GetRelativeRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AIkarusCharacter::SnapTurn(const FInputActionValue& Value)
{
	if(!bIsSnapTurn) return;

	//  Look Functionality
	const float LookValue = Value.Get<float>();
	if(LookValue > 0.f)
	{
		AddControllerYawInput(SnapTurnValue);
	}
	else
	{
		AddControllerYawInput(SnapTurnValue * -1.f);
	}
}

void AIkarusCharacter::SmoothTurn(const FInputActionValue& Value)
{
	if(bIsSnapTurn) return;

	//  Look Functionality
	const FVector2D LookValue = Value.Get<FVector2D>();
	AddControllerYawInput(LookValue.X * TurnRate);
	AddControllerPitchInput(LookValue.Y * TurnRate);
}

void AIkarusCharacter::SitStand()
{
	const FVector CameraHeightOffset = FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	
	if(bIsSitting)
	{
		VRCamera->SetWorldLocation(VRCamera->GetComponentLocation() + CameraHeightOffset);
		bIsSitting = false;
	}
	else
	{
		VRCamera->SetWorldLocation(VRCamera->GetComponentLocation() - CameraHeightOffset);
		bIsSitting = true;
	}
}

void AIkarusCharacter::UpdateCapsuleHalfHeight()
{
	
	static FRotator DeviceRotation = FRotator::ZeroRotator;
	static FVector DevicePosition = FVector::ZeroVector;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

	const float OriginHeightOffset = CapsuleHalfHeight - ((DevicePosition.Z / 2.f) + 5.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight);
	VROrigin->AddRelativeLocation(FVector(0.f, 0.f, OriginHeightOffset));
	CapsuleHalfHeight = (DevicePosition.Z / 2.f) + 5.f;
	
}

void AIkarusCharacter::UpdateCapsulePosition()
{
	FVector CameraOffset = VRCamera->GetComponentLocation() - GetCapsuleComponent()->GetComponentLocation();
	CameraOffset.Z = 0.f;

	GetMovementComponent()->MoveUpdatedComponent(CameraOffset, GetActorRotation(), true);
	VROrigin->AddWorldOffset(UKismetMathLibrary::NegateVector(CameraOffset));
}

void AIkarusCharacter::SetTeleportFade(float FadeValue)
{
	if(FadeMaterialInst)
	{
		if(TeleportFadeSphere->GetMaterial(0) != FadeMaterialInst)
		{
			TeleportFadeSphere->SetMaterial(0, FadeMaterialInst);
		}
		
		FadeMaterialInst->SetScalarParameterValue(TEXT("Opacity"), FadeValue);
	}
}
