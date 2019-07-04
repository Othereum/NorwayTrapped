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

	void Press(UChrStateComp* Comp);
	void Release(UChrStateComp* Comp);
};

USTRUCT()
struct FPostureSwitchAnimData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToStand;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToCrouch;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToProne;

	UAnimMontage* GetSwitchToAnim(const EPosture To) const { return (&ToStand)[static_cast<uint8>(To)]; }
};

USTRUCT()
struct FPostureData : public FStateInputData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight;

	UPROPERTY(EditAnywhere)
	float MeshOffset;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToStand;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToCrouch;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToProne;

	UAnimMontage* GetSwitchToAnim(const EPosture To) const { return (&ToStand)[static_cast<uint8>(To)]; }
	void Press(UChrStateComp* Comp);
};

USTRUCT(BlueprintType)
struct FProneData : public FPostureData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SpeedRatioWhileSwitching = 1.f;

	UPROPERTY(BlueprintReadOnly)
	uint8 bSwitching : 1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NORWAYTRAPPED_API UChrStateComp final : public UActorComponent
{
	GENERATED_BODY()

public:
	UChrStateComp();

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	void SetCapsuleHalfHeight(float Height, float MeshOffset = 0.f) const;
	bool IsOverlapped(float Height) const;
	void Transit();
	bool IsSprinting() const { return bSprinting; }
	void PlayAnimMontage(UAnimMontage* Anim);
	void SetProneSwitchDelegate();

private:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	class ACharacter* const Owner = nullptr;

	UPROPERTY(EditAnywhere)
	FStateInputData Walk;

	UPROPERTY(EditAnywhere)
	FStateInputData Sprint;

	UPROPERTY(EditAnywhere)
	FPostureSwitchAnimData Stand;

	UPROPERTY(EditAnywhere)
	FPostureData Crouch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FProneData Prone;

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
	void TrySetSprintingAndTransit(bool b);
	void SetSprinting_Internal(bool b);
	bool CanSprint() const;
	void ModifyInputScale(float);

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bSprinting : 1;

	UPROPERTY(ReplicatedUsing = OnRep_Posture, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EPosture Posture;
	class FCharacterPosture* PostureState;

	UFUNCTION()
	void OnRep_Posture();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetPosture(EPosture NewPosture);

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UAnimMontage* LastAnim;
};
