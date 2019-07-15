// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "AnimNotify_GunReload.h"
#include "AnimNotify_Bolt.generated.h"

UCLASS()
class UAnimNotify_Bolt : public UAnimNotify_GunReload
{
	GENERATED_BODY()
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
