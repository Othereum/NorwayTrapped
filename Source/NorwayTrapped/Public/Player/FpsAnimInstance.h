// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PostureComponent.h" // EPosture
#include "FpsAnimInstance.generated.h"

UCLASS()
class UFpsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	FName WeaponLeftHandIKSocketName = "IK_LeftHand";

	UPROPERTY(EditAnywhere)
	FName RightHandBoneName = "RightHand";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator Aim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform LeftHandIK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPosture Posture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bSprinting : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bSwitchingProne : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bFABRIK : 1;
};
