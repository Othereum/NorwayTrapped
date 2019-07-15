// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "AnimNotify_EndReload.h"
#include "Gun.h"

void UAnimNotify_EndReload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto Gun = GetGun(MeshComp))
	{
		Gun->EndReload();
	}
}
