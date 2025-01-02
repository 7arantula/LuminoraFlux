// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRBaseControllers.generated.h"

UENUM(BlueprintType)
enum class EHandType : uint8
{
	EHT_None UMETA(DisplayName = "None"),
	EHT_LeftHand UMETA(DisplayName = "LeftHand"),
	EHT_RightHand UMETA(DisplayName = "RightHand")
};

class AIkarusCharacter;
class UVRGrabComponent;
class UVRControllerAnimInstance;
class USkeletalMeshComponent;
class UWidgetInteractionComponent;
class UMotionControllerComponent;
class UNiagaraComponent;

UCLASS()
class LUMINORAFLUX_API AVRBaseControllers : public AActor
{
	GENERATED_BODY()
	
public:

	
	AVRBaseControllers();
	virtual void Tick(float DeltaTime) override;

	//  Public Getter Functions
	FORCEINLINE UMotionControllerComponent* GetLeftMotionController() const { return MotionController; }
	FORCEINLINE UWidgetInteractionComponent* GetLeftWIC() const { return WidgetInteraction; }
	FORCEINLINE UNiagaraComponent* GetTraceRay() const { return TeleportTraceRay; }
	FORCEINLINE UNiagaraComponent* GetTraceRing() const { return TeleportTraceRing; }

	//  Player Input Functions
	void TeleportStarted();
	void TeleportTriggering();
	void TeleportCompleted();
	void SpawnMenu();

	void MenuToggle();

	void LeftGrabStarted();
	void LeftGrabCompleted();
	void RightGrabStarted();
	void RightGrabCompleted();

	UPROPERTY()
	UVRControllerAnimInstance* ControllerAnimInstance;
protected:
	
	virtual void BeginPlay() override;

private:

	/**
	 * Private Variables
	 */
	
	UPROPERTY()
	FRotator LRotation;
	
	UPROPERTY()
	FRotator RRotation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Controller Properties")
	EHandType HandType = EHandType::EHT_None;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AIkarusCharacter* LuminoraFlux;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UVRGrabComponent* HeldComp;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Menu;

	UPROPERTY()
	AActor* MenuRef;

	UPROPERTY()
	bool bTeleportTraceActive;

	UPROPERTY()
	bool bValidTeleportLocation;

	UPROPERTY()
	FTimerHandle DelayHandle;

	UPROPERTY()
	float TeleportTimer;

	UPROPERTY()
	FVector TeleportLoc = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector ProjectedTeleportLocation;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FVector> TeleportTracePathPositions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Teleport Properties")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundTraceType;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport Properties")
	float TeleportLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport Properties")
	float TeleportProjectileRadius = 3.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport Properties")
	float TeleportFadeSpeed = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport Properties")
	bool bUseInterpTeleportRay = true;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport Properties")
	float TeleportInterpSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport Properties")
	float NavMeshCellHeight = 1.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* MotionControllerAim;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetInteractionComponent* WidgetInteraction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ControllerMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TeleportTraceRay;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TeleportTraceRing;

	
	/**
	 * Private Functions
	 */
	
	UVRGrabComponent* GetGrabComponent(UMotionControllerComponent* MotionControllerRef);
	void StartTeleportTrace();
	void TeleportTrace(FVector StartPosition, FVector ForwardVector);
	void EndTeleportTrace();
	void TryTeleport();
	void ExecuteTeleportation();
	void TraceActorByTag();
	void Print(FString&String);
	
};