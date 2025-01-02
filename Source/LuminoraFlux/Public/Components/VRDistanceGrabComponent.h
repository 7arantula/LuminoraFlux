// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRGrabComponent.h"
#include "Components/SceneComponent.h"
#include "VRDistanceGrabComponent.generated.h"


class UVRGrabComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUMINORAFLUX_API UVRDistanceGrabComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UVRDistanceGrabComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	virtual void BeginPlay() override;

private:
	/**
	 * PrivateVariables
	 */
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsDetecting;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> IgnoreActors;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector TraceStartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UVRGrabComponent* TargatedGrabComponent;

	/**
	 * Private Functions
	 */
	void SetDetecting(bool bDetecting);
	UVRGrabComponent* Grab(UVRGrabComponent* TargatedGrabComp, USceneComponent* AttachmentComp, UVRGrabComponent& GrabComp, bool& bIsValid, bool& bIsHeld);
	UVRGrabComponent* GetGrabComponent();
	void SetPrimitiveCompPhysics(UVRGrabComponent* GrabComp, bool bSimulate);
};
