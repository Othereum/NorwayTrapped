// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Gun.generated.h"

UCLASS()
class AGun final : public AWeapon
{
	GENERATED_BODY()

	void Tick(float DeltaSeconds) override;
	bool CanFire() const;
	void HandleFire(float DeltaSeconds);
	void Fire();
	void FireP() override;
	void FireR() override;

	UPROPERTY(EditAnywhere)
	uint8 bAutomatic : 1;

	UPROPERTY(EditAnywhere, meta = (EditCondition = bAutomatic, ClampMin = 1, UIMin = 1, UIMax = 1500))
	uint16 Rpm = 600;

	float FireLag;
};
