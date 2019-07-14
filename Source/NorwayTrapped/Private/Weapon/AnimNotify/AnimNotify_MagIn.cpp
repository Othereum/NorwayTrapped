// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "AnimNotify_MagIn.h"
#include "Gun.h"

void UAnimNotify_MagIn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto Gun = GetGun(MeshComp))
	{
		Gun->MagIn();
	}
}
