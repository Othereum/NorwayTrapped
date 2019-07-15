// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "AnimNotify_Bolt.h"
#include "Gun.h"

void UAnimNotify_Bolt::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (const auto Gun = GetGun(MeshComp))
	{
		Gun->Bolt();
	}
}
