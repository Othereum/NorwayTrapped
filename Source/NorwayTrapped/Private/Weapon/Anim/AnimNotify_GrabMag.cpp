// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "AnimNotify_GrabMag.h"
#include "Gun.h"

void UAnimNotify_GrabMag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto Gun = GetGun(MeshComp))
	{
		Gun->GrabMag();
	}
}
