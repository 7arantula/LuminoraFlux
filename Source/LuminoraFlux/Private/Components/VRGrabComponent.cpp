// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/VRGrabComponent.h"
#include "Character/VRBaseControllers.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"

UVRGrabComponent::UVRGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	GrabType = EGrabType::EGT_Free;
}

void UVRGrabComponent::SetHandleAndArrow(FRotator InterpolatedRotation)
{
	/*
	float Time = GetWorld()->GetDeltaSeconds();
	float InterpSpeed = 8.f;
	
	FRotator TH =  ArrowComponent->GetComponentRotation();
	FRotator TH2 = InterpolatedRotation;
	if(TH.Yaw < Threshold.Yaw+45.f && TH.Yaw > Threshold.Yaw-45.f)
	{
		ArrowComponent->SetWorldRotation(InterpolatedRotation);
	}
	else if(TH.Yaw >= Threshold.Yaw+45.f)
	{
		TH2.Yaw = Threshold.Yaw+44.9f;
		ArrowComponent->SetWorldRotation(TH2);
	}
	else if (TH.Yaw <= Threshold.Yaw-45.f)
	{
		TH2.Yaw = Threshold.Yaw-44.9f;
		ArrowComponent->SetWorldRotation(TH2);
	}
	*/
	ArrowComponent->SetWorldRotation(InterpolatedRotation);
	FRotator ADD = ArrowOriginalRotation-ArrowNewRotation;
	FRotator ERot= ArrowComponent->GetComponentRotation();
	FRotator SUB = HandleOriginalRotation;
	ERot = ERot+ADD+SUB;
	HandleStaticMeshComponent->SetWorldRotation(ERot);
}

void UVRGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(ParentComponent!=nullptr&&GrabType == EGrabType::EGT_Loose)
	{
		FVector SLocation=ParentComponent->GetComponentLocation();
		FVector ELocation=SkeletalMeshComponent->GetComponentLocation();
		double Magnitude = UKismetMathLibrary::Vector_Distance(SLocation,ELocation);
		
		if(	Magnitude>80.f)
		{
			TryRelease(ParentComponent);
		}
	}
	if(GrabType == EGrabType::EGT_Loose && AttachType == EAttachType::EAT_Parent&& SkeletalMeshComponent!= nullptr&&ParentComponent!=nullptr)
	{
		FVector SLocation = SkeletalMeshComponent->GetRelativeLocation();
		FRotator SRotation = SkeletalMeshComponent->GetRelativeRotation();
		FVector ELocation = OriginalRelativeLocation;
		VectorInterpolation(SLocation, ELocation);
		
		float Time = GetWorld()->GetDeltaSeconds();
		float InterpSpeed = 5.f; 
		FRotator InterpolatedRotation = FMath::RInterpTo(SRotation,OriginalRelativeRotation,Time,InterpSpeed);
		SkeletalMeshComponent->SetRelativeRotation(InterpolatedRotation);

		
		const float Tolerance = 0.1f;
		if(FMath::IsNearlyEqual(SLocation.X, ELocation.X, Tolerance) &&
			FMath::IsNearlyEqual(SLocation.Y, ELocation.Y, Tolerance) &&
			FMath::IsNearlyEqual(SLocation.Z, ELocation.Z, Tolerance))
		{
			AttachType = EAttachType::EAT_None;
		}
	}
	if(GrabType == EGrabType::EGT_Loose && AttachType == EAttachType::EAT_OtherActor&& SkeletalMeshComponent!= nullptr)
	{
		
		FVector SLocation = SkeletalMeshComponent->GetRelativeLocation();//GetComponentLocation();
		FVector ELocation = ArrowComponent->GetRelativeLocation();//GetComponentLocation();
		ELocation.Y -=18.f;
		ELocation.Z -=4.5f;
		FRotator ARotation = ArrowComponent->GetComponentRotation();
		FRotator SKMRotation = SkeletalMeshComponent->GetComponentRotation();
		const float Tolerance = 0.1f;
		
		if(FMath::IsNearlyEqual(SLocation.X, ELocation.X, Tolerance)&&
		FMath::IsNearlyEqual(SLocation.Y, ELocation.Y, Tolerance)&&
		FMath::IsNearlyEqual(SLocation.Z, ELocation.Z, Tolerance))
		{
			if(ParentComponent!=nullptr)
			{
				FRotator LookToRotation = UKismetMathLibrary::FindLookAtRotation(ArrowComponent->GetComponentLocation(),ParentComponent->GetComponentLocation());
				FRotator ACopy = ARotation;
				ACopy.Yaw=LookToRotation.Yaw+90.f;
			
				float Time = GetWorld()->GetDeltaSeconds();
				float InterpSpeed = 4.f;

				FRotator MCRotation = ParentComponent->GetComponentRotation();
				MCRotation.Roll = 0.f;
				MCRotation.Pitch = 0.f;
				MCRotation.Yaw = MCRotation.Yaw-90.f;
				
				FRotator InterpolatedRotation = FMath::RInterpTo(ARotation,MCRotation,Time,InterpSpeed);
				SetHandleAndArrow(InterpolatedRotation);
			}
		}
		else
		{
			VectorInterpolation(SLocation, ELocation);
			RotorInterpolation(SKMRotation, ARotation);
		}
	}
}

void UVRGrabComponent::BeginPlay()
{
	Super::BeginPlay();
	SetShouldSimulateOnDrop();
	UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(GetAttachParent());
	PrimitiveComp->SetCollisionProfileName(TEXT("PhysicsActor"), true);
}

void UVRGrabComponent::SetShouldSimulateOnDrop()
{
	const UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(GetAttachParent());
	if(PrimitiveComp->IsAnySimulatingPhysics())
	{
		bSimulateOnDrop = true;
	}
}

void UVRGrabComponent::SetPrimitiveCompPhysics(bool Simulate)
{
	UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(GetAttachParent());
	PrimitiveComp->SetSimulatePhysics(Simulate);
}

bool UVRGrabComponent::TryGrab(USceneComponent* AttachmentComp)
{
	if(GrabType == EGrabType::EGT_None)
	{
		// Do Nothing
	}
	if(GrabType == EGrabType::EGT_Socket)
	{
		SetPrimitiveCompPhysics(false);
		ParentComponent=AttachmentComp;
		AttachParentToSocket();
		bIsHeld = true;
	}
	if(GrabType == EGrabType::EGT_Free)
	{
		SetPrimitiveCompPhysics(false);
		AttachParentToMotionController(AttachmentComp);
		bIsHeld = true;
	}
	if(GrabType == EGrabType::EGT_Snap)
	{
		SetPrimitiveCompPhysics(false);
		AttachParentToMotionController(AttachmentComp);
		bIsHeld = true;

		FHitResult HitResult;

		const FRotator NewRelativeRot = UKismetMathLibrary::NegateRotator(GetRelativeRotation());
		GetAttachParent()->SetRelativeRotation(NewRelativeRot, false, &HitResult, ETeleportType::TeleportPhysics);

		const FVector ControllerLocation = AttachmentComp->GetComponentLocation();
		const FVector TempLocation = (GetComponentLocation() - GetAttachParent()->GetComponentLocation()) * -1.f;
		const FVector NewLocation = ControllerLocation + TempLocation;
		GetAttachParent()->SetWorldLocation(NewLocation, false, &HitResult, ETeleportType::TeleportPhysics);
	}
	if(GrabType == EGrabType::EGT_Loose)
	{
		SetPrimitiveCompPhysics(false);
		AttachSKMToParent(AttachmentComp);
		bIsHeld = true;
		LooseIsHeld = true;
		ParentComponent=AttachmentComp;
	}
	else
	{
		// Some Functionality to be added in the future.
	}

	if(bIsHeld)
	{
		return true;
	}
	return false;
}

bool UVRGrabComponent::TryRelease(USceneComponent*ParentComp)
{
	TrySimulateOnDrop();
	if(GrabType==EGrabType::EGT_Loose)
	{
		DetachSKMHand(ParentComp);
	}
	if(bIsHeld)
	{
		return false;
	}
	bIsHeld = false;
	LooseIsHeld = false;
	NoneGrabbed();
	return true;
}

void UVRGrabComponent::StopPull()
{
	if(bIsPulled)
	{
		bIsPulled = false;

		if(!bIsHeld)
		{
			TrySimulateOnDrop();
		}
	}
}

bool UVRGrabComponent::TryPull()
{
	TArray<UVRGrabComponent*> Components;
	GetOwner()->GetComponents<UVRGrabComponent>(Components);
	for(const UVRGrabComponent* Component : Components)
	{
		if(Component->bIsPulled)
		{
			return false;
		}
	}

	bIsPulled = true;
	SetPrimitiveCompPhysics(false);
	return true;
}

void UVRGrabComponent::TrySimulateOnDrop()
{
	if(bSimulateOnDrop)
	{
		SetPrimitiveCompPhysics(true);
		bIsHeld = false;
	}
	else
	{
		GetOwner()->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));
		bIsHeld = false;
	}
}

void UVRGrabComponent::AttachParentToMotionController(USceneComponent* AttachmentComp)
{
	const FAttachmentTransformRules Rules =  FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	
	TArray<FName> ActorTags = GetOwner()->Tags;
	if(ActorTags.Num() > 0)
	{
		const FName FoundSocketName = ActorTags[0];
		if(FoundSocketName != FName(NAME_None))
		{
			GetOwner()->AttachToComponent(AttachmentComp, Rules, FoundSocketName);
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Black, FString::Printf(TEXT("SocketName: %s"), *FoundSocketName.ToString()));
		}
		else
		{
			GetOwner()->AttachToComponent(AttachmentComp, Rules);
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Black, TEXT("Attached with No Socket"));
		}
	}
	else
	{
		GetOwner()->AttachToComponent(AttachmentComp, Rules);
		//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Black, TEXT("Attached with No Socket"));
	}
}

void UVRGrabComponent::AttachParentToSocket()
{
	if(bIsHeld==true)
	{
		return;
	}
	const FAttachmentTransformRules Rules = FAttachmentTransformRules(
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::KeepWorld,
	true
		
);
	
	if (AVRBaseControllers* ControllerParent = Cast<AVRBaseControllers>(ParentComponent->GetOwner()))
	{
		SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ControllerParent->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		
		
		/*if (PoseType == EPoseType::EPT_ThumbHold)
		{
			GetOwner()->AttachToComponent(SkeletalMeshComponent,Rules,("ThumbHold"));
		}
		else if(PoseType == EPoseType::EPT_FlatPalm)
		{
			GetOwner()->AttachToComponent(SkeletalMeshComponent,Rules,("FlatPalm"));
		}
		else if(PoseType == EPoseType::EPT_AllFingureHold)
		{
			GetOwner()->AttachToComponent(SkeletalMeshComponent,Rules,("AllFingureHold"));
		}
		*/
		
		switch (PoseType)
		{
		case EPoseType::EPT_ThumbHold:
			{
				GetOwner()->AttachToComponent(SkeletalMeshComponent,Rules,("ThumbHold"));
				break;
			}
		case EPoseType::EPT_AllFingureHold:
			{
				GetOwner()->AttachToComponent(SkeletalMeshComponent,Rules,("AllFingureHold"));
				break;
			}
		case EPoseType::EPT_FlatPalm:
			{
				GetOwner()->AttachToComponent(SkeletalMeshComponent,Rules,("FlatPalm"));
				break;
			}
		default:
			{
				GetOwner()->AttachToComponent(SkeletalMeshComponent,Rules,("FlatPalm"));
				break;
			}
		}
	}
}

void UVRGrabComponent::AttachSKMToParent(USceneComponent* ParentComp)
{
	if(bIsHeld==true)
	{
		return;
	}

	const FAttachmentTransformRules Rules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		true
		
	);

	if (AVRBaseControllers* ControllerParent = Cast<AVRBaseControllers>(ParentComp->GetOwner()))
	{
		SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ControllerParent->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		
		OriginalRelativeRotation=SkeletalMeshComponent->GetRelativeRotation();
		OriginalRelativeLocation = SkeletalMeshComponent->GetRelativeLocation();
		OriginalRotation = SkeletalMeshComponent->GetComponentRotation();
		ArrowComponent = Cast<UArrowComponent>(GetOwner()->GetComponentByClass(UArrowComponent::StaticClass()));
		HandleStaticMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		FRotator ACR=ArrowComponent->GetRelativeRotation();
		HandleOriginalRotation = HandleStaticMeshComponent->GetRelativeRotation();
		ArrowOriginalRotation = ArrowComponent->GetComponentRotation();
		ACR.Yaw=OriginalRotation.Yaw ;
		Threshold = ACR;
		ArrowComponent->SetRelativeRotation(ACR);
		ArrowNewRotation = ArrowComponent->GetComponentRotation();
		SkeletalMeshComponent->AttachToComponent(ArrowComponent, Rules);
		AttachType=EAttachType::EAT_OtherActor;
		LooseIsHeld = true;
	}
}

void UVRGrabComponent::Print(FString& String)
{
	GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Emerald,String);
}

void UVRGrabComponent::DetachSKMHand(USceneComponent* ParentComp)
{
	const FAttachmentTransformRules Rules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		true);
		SkeletalMeshComponent->AttachToComponent(ParentComp,Rules);
		AttachType=EAttachType::EAT_Parent;
		ArrowComponent->SetWorldRotation(ArrowOriginalRotation);
}

void UVRGrabComponent::RotorInterpolation(FRotator& SRotation, FRotator& ERotation)
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float InterpSpeed = 5.f; 
	FRotator InterpolatedRotation = FMath::RInterpTo(SRotation,ERotation,DeltaTime,InterpSpeed);
	SkeletalMeshComponent->SetWorldRotation(InterpolatedRotation);
}

void UVRGrabComponent::VectorInterpolation(FVector&StartLocation, FVector&EndLocation)
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float InterpSpeed = 5.f; 
	
	FVector InterpolatedLocation = FMath::VInterpTo(StartLocation, EndLocation, DeltaTime,InterpSpeed);

	if(AttachType==EAttachType::EAT_Parent)
	{
		SkeletalMeshComponent->SetRelativeLocation(InterpolatedLocation);
	}
	else if(AttachType==EAttachType::EAT_OtherActor)
	{
		SkeletalMeshComponent->SetRelativeLocation(InterpolatedLocation);
	}
}

void UVRGrabComponent::LeftHand()
{
	GrabHandType = EGrabHandType::EGHT_LeftHand;
}

void UVRGrabComponent::RightHand()
{
	GrabHandType = EGrabHandType::EGHT_RightHand;
}

void UVRGrabComponent::NoneGrabbed()
{
	GrabHandType = EGrabHandType::EGHT_Nothing; 
}


