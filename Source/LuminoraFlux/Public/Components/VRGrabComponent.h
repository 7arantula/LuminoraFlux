// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BaseAllEnum.h"
#include "VRGrabComponent.generated.h"

UENUM(BlueprintType)
enum class EGrabType : uint8
{
	EGT_None UMETA(DisplayName = "None"),
	EGT_Free UMETA(DisplayName = "FreeGrab"),
	EGT_Snap UMETA(DisplayName = "SnapGrab"),
	EGT_Distance UMETA(DisplayName = "DistanceGrab"),
	EGT_Socket UMETA(DisplayName = "SocketGrab"),
	EGT_Loose UMETA(DisplayName = "LooseGrab")
};
UENUM()
enum class EAttachType : uint8
{
	EAT_None UMETA(DisplayName = "None"),
	EAT_OtherActor UMETA(DisplayName = "FreeGrab"),
	EAT_Parent UMETA(DisplayName = "SnapGrab")
};
UENUM()
enum class EGrabHandType : uint8
{
	EGHT_Nothing UMETA(DisplayName = "Nothing"),
	EGHT_LeftHand UMETA(DisplayName = "GrabLeft"),
	EGHT_RightHand UMETA(DisplayName = "GrabRight")
};

class UMotionControllerComponent;
class UArrowComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LUMINORAFLUX_API UVRGrabComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UVRGrabComponent();
	void SetHandleAndArrow(FRotator InterpolatedRotation);
	bool TryGrab(USceneComponent* AttachmentComp);
	void TrySimulateOnDrop();
	bool Grabbed;
	bool TryRelease(USceneComponent* ParentComp);
	void StopPull();
	bool TryPull();
	UPROPERTY()
	bool LooseIsHeld;
	FRotator OriginalRotation;
	FRotator OriginalRelativeRotation;
	FVector OriginalRelativeLocation;
	FRotator ArrowOriginalRotation;
	FRotator ArrowNewRotation;
	FRotator Threshold;
	FRotator HandleOriginalRotation;

	
	
	void LeftHand();
	void RightHand();
	void NoneGrabbed();
	
	UPROPERTY(EditAnywhere, Category = "Grab Parameters")
	EGrabHandType GrabHandType;
	UPROPERTY(EditAnywhere, Category = "Grab Parameters", meta = (AllowPrivateAccess))
	EPoseType PoseType;

	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY()
	bool bIsPulled;

	UPROPERTY()
	UArrowComponent*ArrowComponent;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY()
	USceneComponent*ParentComponent;
	UPROPERTY()
	UStaticMeshComponent* HandleStaticMeshComponent;
	

private:

	//  Private Variables
	UPROPERTY()
	bool bSimulateOnDrop;

	UPROPERTY()
	bool bIsHeld;

	//UPROPERTY()
	//bool RotateWhom;

	UPROPERTY(EditAnywhere, Category = "Grab Parameters", meta = (AllowPrivateAccess))
	EGrabType GrabType;
	
	UPROPERTY(EditAnywhere, Category = "Grab Parameters", meta = (AllowPrivateAccess))
	EAttachType AttachType;



	//  Private Functions
	void SetShouldSimulateOnDrop();
	void SetPrimitiveCompPhysics(bool Simulate);
	void AttachParentToMotionController(USceneComponent* AttachmentComp);
	void AttachParentToSocket();
	void AttachSKMToParent(USceneComponent* ParentComp);
	void Print(FString&String);
	void DetachSKMHand(USceneComponent*ParentComp);
	void RotorInterpolation(FRotator& SRotation, FRotator& ERotation);
	void VectorInterpolation (FVector& StartLocation,FVector& EndLocation);
	
	
};
