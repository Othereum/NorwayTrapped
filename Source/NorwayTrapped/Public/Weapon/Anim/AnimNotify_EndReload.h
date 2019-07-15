// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Weapon/Anim/AnimNotify_GunReload.h"
#include "AnimNotify_EndReload.generated.h"

UCLASS()
class UAnimNotify_EndReload : public UAnimNotify_GunReload
{
	GENERATED_BODY()
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
