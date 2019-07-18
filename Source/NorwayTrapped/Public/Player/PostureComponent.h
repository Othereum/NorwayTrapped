// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PostureComponent.generated.h"

UENUM(BlueprintType)
enum class EPosture : uint8
{
	Stand, Crouch, Prone
};

class UPostureComponent;
class UAnimMontage;

USTRUCT()
struct FStateInputData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SpeedRatio = 1.f;

	UPROPERTY(EditAnywhere)
	uint8 bToggle : 1;

	uint8 bPressed : 1;

	void Press(UPostureComponent* Comp);
	void Release(UPostureComponent* Comp);
};

USTRUCT()
struct FSprintData : public FStateInputData
{
	GENERATED_BODY()
	void Press(UPostureComponent* Comp);
};

USTRUCT()
struct FStandData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToCrouch;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToProne;

	UAnimMontage* GetSwitchToAnim(const EPosture To) const { return (&ToCrouch)[static_cast<uint8>(To) - 1]; }
};

USTRUCT()
struct FPostureData : public FStateInputData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight;

	UPROPERTY(EditAnywhere)
	float MeshOffset;

	void Press(UPostureComponent* Comp);
};

USTRUCT(BlueprintType)
struct FCrouchData : public FPostureData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToStand;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToProne;

	UAnimMontage* GetSwitchToAnim(const EPosture To) const { return To == EPosture::Stand ? ToStand : ToProne; }
};

USTRUCT(BlueprintType)
struct FProneData : public FPostureData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToStand;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToCrouch;

	UPROPERTY(EditAnywhere)
	float SpeedRatioWhileSwitching = 1.f;

	UPROPERTY(BlueprintReadOnly)
	uint8 bSwitching : 1;

	UAnimMontage* GetSwitchToAnim(const EPosture To) const { return (&ToStand)[static_cast<uint8>(To)]; }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPostureComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UPostureComponent();

private:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	void Transit();
	void PlayAnimMontage(UAnimMontage* Anim) const;
	void PlayPostureSwitchAnim(UAnimMontage* Anim);
	void SetProneSwitchDelegate();

	void SetCapsuleHalfHeight(float Height, float MeshOffset = 0.f) const;
	bool IsOverlapped(float Height) const;

	bool IsSprinting() const { return bSprinting; }
	EPosture GetPostureEnum() const { return Posture; }

	class AFpsCharacter* const Owner = nullptr;

	UPROPERTY(EditAnywhere)
	FStateInputData Walk;

	UPROPERTY(EditAnywhere)
	FSprintData Sprint;

	UPROPERTY(EditAnywhere)
	FStandData Stand;

	UPROPERTY(EditAnywhere)
	FCrouchData Crouch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FProneData Prone;

private:
	bool CanSprint() const;
	void CheckState();
	void TrySetSprintingAndTransit(bool b);
	void SetSprinting_Internal(bool b);
	void ModifyInputScale(float);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool b);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetPosture(EPosture NewPosture);

	UFUNCTION()
	void OnRep_Posture();

	void WalkPressed();
	void WalkReleased();
	void SprintPressed();
	void SprintReleased();
	void CrouchPressed();
	void CrouchReleased();
	void PronePressed();
	void ProneReleased();

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bSprinting : 1;

	UPROPERTY(ReplicatedUsing = OnRep_Posture, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EPosture Posture;
	class FCharacterPosture* PostureState;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UAnimMontage* LastAnim;
};
