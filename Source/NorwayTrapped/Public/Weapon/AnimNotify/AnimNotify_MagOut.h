// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "AnimNotify_GunReload.h"
#include "AnimNotify_MagOut.generated.h"

UCLASS()
class UAnimNotify_MagOut : public UAnimNotify_GunReload
{
	GENERATED_BODY()
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
