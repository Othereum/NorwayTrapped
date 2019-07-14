// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "AnimNotify_MagOut.h"
#include "Gun.h"

void UAnimNotify_MagOut::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto Gun = GetGun(MeshComp))
	{
		Gun->MagOut();
	}
}
