// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "AnimNotify_GunReload.h"
#include "Components/SkeletalMeshComponent.h"
#include "FpsCharacter.h"
#include "Gun.h"
#include "WeaponComponent.h"

AGun* UAnimNotify_GunReload::GetGun(USkeletalMeshComponent* MeshComp) const
{
	if (const auto Owner = Cast<AFpsCharacter>(MeshComp->GetOwner()))
	{
		return Cast<AGun>(Owner->GetWeaponComponent()->GetActiveWeapon());
	}
	return nullptr;
}
