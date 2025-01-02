// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VRBaseControllers.h"
#include "Character/IkarusCharacter.h"
#include "Components/VRGrabComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Character/VRControllerAnimInstance.h"
#include "Components/WidgetInteractionComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AVRBaseControllers::AVRBaseControllers()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GroundTraceType.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	
	//  Construct Motion Controllers
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionController"));
	MotionController->SetupAttachment(GetRootComponent());
	MotionController->MotionSource = TEXT("Left");
	MotionController->PlayerIndex = 0;
	
	//  Construct Motion Controllers Aim
	MotionControllerAim = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftAim"));
	MotionControllerAim->SetupAttachment(GetRootComponent());
	MotionControllerAim->MotionSource = TEXT("LeftAim");
	MotionControllerAim->PlayerIndex = 0;

	//  Construct Widget Interaction Components
	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("LeftWidgetInteraction"));
	WidgetInteraction->SetupAttachment(MotionControllerAim);
	WidgetInteraction->PointerIndex = 0;

	//  Setup Niagara System
	TeleportTraceRay = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportRay"));
	TeleportTraceRay->SetupAttachment(MotionControllerAim);
	TeleportTraceRay->SetVisibility(false);

	TeleportTraceRing = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportRing"));
	TeleportTraceRing->SetupAttachment(GetRootComponent());
	TeleportTraceRing->SetVisibility(false);

	//  Construct Hand Skeletal Meshes
	ControllerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftMotionControllerMesh"));
	ControllerMesh->SetupAttachment(MotionController);
	ControllerMesh->bCastDynamicShadow = false;
	
}
void AVRBaseControllers::BeginPlay()
{
	Super::BeginPlay();

	LuminoraFlux = Cast<AIkarusCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	//  Save Anim Instances
	ControllerAnimInstance = Cast<UVRControllerAnimInstance>(ControllerMesh->GetAnimInstance());
	if(HandType == EHandType::EHT_LeftHand)
	{
		MotionController->MotionSource = TEXT("Left");
		MotionController->PlayerIndex = 0;

		MotionControllerAim->MotionSource = TEXT("LeftAim");
		MotionControllerAim->PlayerIndex = 0;

		WidgetInteraction->PointerIndex = 0;
	}
	else if (HandType == EHandType::EHT_RightHand)
	{
		MotionController->MotionSource = TEXT("Right");
		MotionController->PlayerIndex = 1;

		MotionControllerAim->MotionSource = TEXT("RightAim");
		MotionControllerAim->PlayerIndex = 1;

		WidgetInteraction->PointerIndex = 1;

		TeleportTraceRay->DestroyComponent();
		TeleportTraceRay->SetActive(false);
	}
	else
	{
		Destroy();
	}

	if(LuminoraFlux->bIsInVR)
	{
		MotionController->SetHiddenInGame(true);
	}
}
void AVRBaseControllers::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AVRBaseControllers::TeleportStarted()
{
	StartTeleportTrace();
}
void AVRBaseControllers::TeleportTriggering()
{
	if(!DelayHandle.IsValid())
	{
		if(bTeleportTraceActive)
		{
			if(LuminoraFlux->bIsInVR)
			{
				const FRotator PawnRotation = LuminoraFlux->GetControlRotation();
				const FVector ControllerLocation = MotionController->GetComponentLocation();
				const FVector ControllerForwardVec = WidgetInteraction->GetForwardVector();

				const FVector RotatedForwardVector = PawnRotation.RotateVector(ControllerForwardVec);
		
				TeleportTrace(ControllerLocation, RotatedForwardVector);
			}
			else
			{
				const FVector ControllerLocation = LuminoraFlux->GetVRCamera()->GetComponentLocation();
				const FVector ControllerForwardVec = LuminoraFlux->GetVRCamera()->GetForwardVector();
				TeleportTrace(ControllerLocation, ControllerForwardVec);
			}
		}
	}
}
void AVRBaseControllers::TeleportCompleted()
{
	EndTeleportTrace();
	TryTeleport();
}

void AVRBaseControllers::SpawnMenu()
{
	MenuRef = GetWorld()->SpawnActor(BP_Menu);

	const FAttachmentTransformRules SnapRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	MenuRef->AttachToComponent(MotionController, SnapRules);
}

void AVRBaseControllers::MenuToggle()
{
	if(HandType == EHandType::EHT_LeftHand)
	{
		if(MenuRef == nullptr)
		{
			SpawnMenu();
		}
		else
		{
			MenuRef->Destroy();
		}
	}
}

void AVRBaseControllers::LeftGrabStarted()
{
	LRotation = ControllerMesh->GetRelativeRotation();
	if(UVRGrabComponent* GrabComp = GetGrabComponent(MotionController))
	{
		if(GrabComp->GrabHandType != EGrabHandType::EGHT_RightHand)
		{
			if(GrabComp->TryGrab(MotionController))
			{
				ControllerAnimInstance->LeftGrabbed=true;
				ControllerAnimInstance->PoseType = GrabComp->PoseType;
				//if(GrabComp->LooseIsHeld==true)
				HeldComp = GrabComp;
				GrabComp->LeftHand();
			}
		}
	}
}
void AVRBaseControllers::LeftGrabCompleted()
{
	if(HeldComp)
	{
		if(HeldComp->GrabHandType != EGrabHandType::EGHT_RightHand)
		{
			if(HeldComp->TryRelease(MotionController))
			{
				ControllerMesh->SetRelativeRotation(LRotation);
				HeldComp = nullptr;
				ControllerAnimInstance->LeftGrabbed=false;
				ControllerAnimInstance->PoseType = EPoseType::EPT_Nothing;
				//HeldComp->NoneGrabbed();
			}
		}
	}
}
void AVRBaseControllers::RightGrabStarted()
{
	RRotation = ControllerMesh->GetRelativeRotation();
	if(UVRGrabComponent* GrabComp = GetGrabComponent(MotionController))
	{
		if(GrabComp->GrabHandType != EGrabHandType::EGHT_LeftHand)
		{
			if(GrabComp->TryGrab(MotionController))
			{
				ControllerAnimInstance->RightGrabbed=true;
				ControllerAnimInstance->PoseType = GrabComp->PoseType;
				//if(GrabComp->LooseIsHeld==true)
				HeldComp = GrabComp;
				GrabComp->RightHand();
			}
		}
	}
}
void AVRBaseControllers::RightGrabCompleted()
{
	if(HeldComp)
	{
		if(HeldComp->GrabHandType != EGrabHandType::EGHT_LeftHand)
		{
			if(HeldComp->TryRelease(MotionController))
			{
				ControllerMesh->SetRelativeRotation(RRotation);
				HeldComp = nullptr;
				ControllerAnimInstance->RightGrabbed=false;
				//HeldComp->NoneGrabbed();
				ControllerAnimInstance->PoseType = EPoseType::EPT_Nothing;
			}
		}
	}
}
UVRGrabComponent* AVRBaseControllers::GetGrabComponent(UMotionControllerComponent* MotionControllerRef)
{
	const FVector GrabPosition = MotionControllerRef->GetComponentLocation();
	constexpr float GrabRadius = 8.f;
	constexpr float NearestCompDist = 5000.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	TArray<AActor*> ActorsToIgnore;
	FHitResult TraceResult;

	// Trace for Nearby Grab Objects
	if(UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),
		GrabPosition, GrabPosition, GrabRadius, TraceObjects,
		false, ActorsToIgnore, EDrawDebugTrace::None, TraceResult,
		true)
	)
	{
		TArray<UVRGrabComponent*> GrabComponents;
		TraceResult.GetActor()->GetComponents<UVRGrabComponent>(GrabComponents);
		
		if(GrabComponents.Num() > 0)
		{
			for(UVRGrabComponent* Component : GrabComponents)
			{
				float FoundCompDist = UKismetMathLibrary::VSizeSquared(Component->GetComponentLocation() - GrabPosition);
				if(FoundCompDist <= NearestCompDist)
				{
					FoundCompDist = NearestCompDist;
					return Component;
				}
			}
		}
	}
	return nullptr;
}
/**
 * Teleport Functions
 */
void AVRBaseControllers::StartTeleportTrace()
{
	bTeleportTraceActive = true;
	if(TeleportTraceRay && TeleportTraceRing)
	{
		TeleportTraceRay->SetVisibility(true);
		TeleportTraceRing->SetVisibility(true);
	}
}
void AVRBaseControllers::TeleportTrace(FVector StartPosition, FVector ForwardVector)
{
	//  Constructing Local Variables
	FHitResult TraceHitResult;
	FVector OutLastTraceDest;
	static FVector TraceVector;
	const FVector LaunchVelocity = TraceVector * TeleportLaunchSpeed;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	if(bUseInterpTeleportRay)
	{
		TraceVector = FMath::VInterpTo(TraceVector, ForwardVector, GetWorld()->GetDeltaSeconds(), TeleportInterpSpeed);
	}
	else
	{
		TraceVector = ForwardVector;
	}

	//  Predict Projectile Path
	UGameplayStatics::Blueprint_PredictProjectilePath_ByObjectType(GetWorld(),
		TraceHitResult, TeleportTracePathPositions, OutLastTraceDest, StartPosition,
		LaunchVelocity, true, TeleportProjectileRadius, GroundTraceType, false,
		ActorsToIgnore, EDrawDebugTrace::None, 0.f);
	
	//  Calculate Projected Location
	TeleportTracePathPositions.Insert(StartPosition, 0);
	if(UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation ProjectedNavLocation;
		if(NavSystem->ProjectPointToNavigation(TraceHitResult.Location, ProjectedNavLocation) && TraceHitResult.Location != FVector::ZeroVector)
		{
			ProjectedTeleportLocation = ProjectedNavLocation.Location;
			ProjectedTeleportLocation = FVector(ProjectedTeleportLocation.X, ProjectedTeleportLocation.Y, ProjectedTeleportLocation.Z + NavMeshCellHeight);
			bValidTeleportLocation = true;
		}
		else
		{
			bValidTeleportLocation = false;
			//ProjectedTeleportLocation = GetActorLocation();
		}
	}

	//  Update Teleport Niagara Systems
	TeleportTraceRing->SetWorldLocation(ProjectedTeleportLocation);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportTraceRay, TEXT("User.PointArray"), TeleportTracePathPositions);
}
void AVRBaseControllers::EndTeleportTrace()
{
	bTeleportTraceActive = false;
	if(TeleportTraceRay && TeleportTraceRing)
	{
		TeleportTraceRay->SetVisibility(false);
		TeleportTraceRing->SetVisibility(false);
	}
}
void AVRBaseControllers::TryTeleport()
{
	if(HandType == EHandType::EHT_LeftHand)
	{
		if(bValidTeleportLocation)
		{
			const float DelayTime = GetWorld()->GetDeltaSeconds();
			const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ExecuteTeleportation);
			GetWorld()->GetTimerManager().SetTimer(DelayHandle, TimerDelegate, DelayTime, true);
		}
	}

	
}
void AVRBaseControllers::ExecuteTeleportation()
{
	
	if(HandType == EHandType::EHT_LeftHand)
	{
		if(bValidTeleportLocation)
		{
			if(TeleportTimer < 1.f)
			{
				LuminoraFlux->SetTeleportFade(TeleportTimer);
				TeleportTimer += TeleportFadeSpeed;
			}
			
			else
			{
				LuminoraFlux->SetTeleportFade(TeleportTimer);
				bValidTeleportLocation = false;
				
				const FVector CamLoc = LuminoraFlux->GetVRCamera()->GetRelativeLocation() * FVector(1.f, 1.f, 0.f);
				const FVector CurrentLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(FRotator(0.f, LuminoraFlux->GetActorRotation().Yaw, 0.f)), CamLoc);
				TeleportLoc = ProjectedTeleportLocation - CurrentLoc;
				LuminoraFlux->TeleportTo(TeleportLoc, FRotator(0.f,  LuminoraFlux->GetActorRotation().Yaw, 0.f));
			}
		}
		else
		{
			TeleportTimer -= TeleportFadeSpeed;
			LuminoraFlux->SetTeleportFade(TeleportTimer);
			if(TeleportTimer <= 0.f)
			{
				GetWorld()->GetTimerManager().ClearTimer(DelayHandle);
				TeleportTimer = 0.f;
			}
		}
	}
}
void AVRBaseControllers::TraceActorByTag()
{
	// Set up the trace parameters
	FCollisionQueryParams TraceParams(FName(TEXT("TraceInsideSphere")), true, nullptr);

	//for (int32 SampleIndex = 0; SampleIndex < 100; ++SampleIndex)
	//{
		FVector RandomDirection = FMath::VRand();
		FVector SphereCenter = ControllerMesh->GetComponentLocation();
		
		FVector TraceStart = SphereCenter + RandomDirection * 50.f;
		FVector TraceEnd = SphereCenter;
		
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, TraceStart, TraceEnd, FCollisionObjectQueryParams::AllObjects, TraceParams);
		
		if (bHit)
		{
			if (HitResult.GetActor() && HitResult.GetActor()->ActorHasTag("Knobs"))
			{
				const FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
				UStaticMeshComponent* HitStaticMeshComponent = nullptr;
				if (AStaticMeshActor* HitStaticMeshActor = Cast<AStaticMeshActor>(HitResult.GetActor()))
				{
					// Get the StaticMeshComponent from the StaticMeshActor
					HitStaticMeshComponent = HitStaticMeshActor->GetStaticMeshComponent();
					FString ActorName = HitStaticMeshActor->GetName();
					Print(ActorName);
				}
				ControllerMesh->AttachToComponent(HitStaticMeshComponent,Rules);
			}
		}
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 2.0f, 0, 1.0f);
	//}
}
void AVRBaseControllers::Print(FString & String)
{
	//GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Emerald,String);
}


