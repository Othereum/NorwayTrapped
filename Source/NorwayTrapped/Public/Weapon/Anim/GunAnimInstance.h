// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunAnimInstance.generated.h"

UCLASS()
class UGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	void NativeUpdateAnimation(float DeltaSeconds) override;
	class AGun* GetGun() const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bEmpty : 1;
};
