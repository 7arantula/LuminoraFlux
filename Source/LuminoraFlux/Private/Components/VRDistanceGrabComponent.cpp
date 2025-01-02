// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/VRDistanceGrabComponent.h"

#include "Components/VRGrabComponent.h"
#include "Kismet/KismetMathLibrary.h"

UVRDistanceGrabComponent::UVRDistanceGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVRDistanceGrabComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVRDistanceGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsDetecting)
	{
		IgnoreActors.Empty();
		TargatedGrabComponent = nullptr;
		
	}
}

void UVRDistanceGrabComponent::SetDetecting(bool bDetecting)
{
	bIsDetecting = bDetecting;
}

UVRGrabComponent* UVRDistanceGrabComponent::Grab(UVRGrabComponent* TargatedGrabComp,
	USceneComponent* AttachmentComp, UVRGrabComponent& GrabComp, bool& bIsValid, bool& bIsHeld)
{
	if(!TargatedGrabComponent)
	{
		bIsValid = false;
		bIsHeld = false;
		return TargatedGrabComp;
	}
	bIsDetecting = false;

	const float InterpSpeed = 10.f;
	
	UVRGrabComponent* LocalGrabComp = TargatedGrabComp;
	SetPrimitiveCompPhysics(LocalGrabComp, false);

	static FVector DestLocation = FMath::VInterpTo(TargatedGrabComp->GetOwner()->GetActorLocation(),
		AttachmentComp->GetComponentLocation(), GetWorld()->GetDeltaSeconds(), InterpSpeed);
	
	TargatedGrabComp->GetOwner()->SetActorLocationAndRotation(DestLocation, TargatedGrabComp->GetOwner()->GetActorRotation());

	const FVector TargetDistance = TargatedGrabComp->GetOwner()->GetActorLocation() - DestLocation;
	if(UKismetMathLibrary::Vector_IsNearlyZero(TargetDistance, 0.1f))
	{
		LocalGrabComp->TryGrab(AttachmentComp);
		TargatedGrabComp = nullptr;

		bIsValid = true;
		bIsHeld = true;

		//This need to be fixed
		//GrabComp = LocalGrabComp;
		return TargatedGrabComp;
	}
	else
	{
		bIsValid = false;
		bIsHeld = false;
		return TargatedGrabComp;
	}
}

UVRGrabComponent* UVRDistanceGrabComponent::GetGrabComponent()
{
	return TargatedGrabComponent;
}

void UVRDistanceGrabComponent::SetPrimitiveCompPhysics(UVRGrabComponent* GrabComp, bool bSimulate)
{
	UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(GrabComp->GetAttachParent());
	if(PrimitiveComp)
	{
		PrimitiveComp->SetSimulatePhysics(bSimulate);
	}
}