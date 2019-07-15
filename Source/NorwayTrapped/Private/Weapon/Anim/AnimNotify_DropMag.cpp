// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "AnimNotify_DropMag.h"
#include "Gun.h"

void UAnimNotify_DropMag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto Gun = GetGun(MeshComp))
	{
		Gun->DropMag();
	}
}
