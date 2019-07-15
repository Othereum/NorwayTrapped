// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "GunAnimInstance.h"
#include "FpsCharacter.h"
#include "Gun.h"
#include "WeaponComponent.h"

void UGunAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (const auto Gun = GetGun())
	{
		bEmpty = Gun->GetClip() == 0;
	}
}

AGun* UGunAnimInstance::GetGun() const
{
	return Cast<AGun>(GetSkelMeshComponent()->GetOwner());
}
