// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChrStateComp.generated.h"

UENUM(BlueprintType)
enum class EPosture : uint8
{
	Stand, Crouch, Prone
};

class UChrStateComp;

USTRUCT()
struct FStateInputData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SpeedRatio = 1.f;

	UPROPERTY(EditAnywhere)
	FName InputName;

	UPROPERTY(EditAnywhere)
	uint8 bToggle : 1;

	uint8 bPressed : 1;

	void Press(UChrStateComp* Comp);
	void Release(UChrStateComp* Comp);
};

USTRUCT()
struct FPostureData : public FStateInputData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight;
};

DECLARE_EVENT_OneParam(UChrStateComp, FChrStateCompTickEvent, float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NORWAYTRAPPED_API UChrStateComp final : public UActorComponent
{
	GENERATED_BODY()

public:
	UChrStateComp();

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	void SetCapsuleHalfHeight(float Height) const;
	bool IsOverlapped(float Height) const;
	void Transit();

private:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	class ACharacter* const Owner = nullptr;

	FChrStateCompTickEvent TickEvent;

	UPROPERTY(EditAnywhere)
	FName MoveForwardInputAxisName = "MoveForward";

	UPROPERTY(EditAnywhere)
	FStateInputData Walk;

	UPROPERTY(EditAnywhere)
	FStateInputData Sprint;

	UPROPERTY(EditAnywhere)
	FPostureData Crouch;

	UPROPERTY(EditAnywhere)
	FPostureData Prone;

private:
	void WalkPressed();
	void WalkReleased();
	void SprintPressed();
	void SprintReleased();
	void CrouchPressed();
	void CrouchReleased();
	void PronePressed();
	void ProneReleased();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool b);
	void SetSprinting(bool b);
	void SetSprinting_Internal(bool b);
	bool CanSprint() const;
	void WalkIfCan(float);

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bSprinting : 1;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EPosture Posture;

	class FCharacterPosture* PostureState;
};
