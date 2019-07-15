// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_GunReload.generated.h"

UCLASS(Abstract)
class NORWAYTRAPPED_API UAnimNotify_GunReload : public UAnimNotify
{
	GENERATED_BODY()

protected:
	class AGun* GetGun(USkeletalMeshComponent* MeshComp) const;
};
