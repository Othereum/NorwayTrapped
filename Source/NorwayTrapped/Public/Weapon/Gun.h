// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Gun.generated.h"

UCLASS()
class AGun final : public AWeapon
{
	GENERATED_BODY()

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CanFire() const;
	void HandleFire(float DeltaSeconds);
	void Fire();
	void FireP() override;
	void FireR() override;
	void StartFire();
	void StopFire();
	void Reload() override;

	void Shoot();

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Clip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true, ClampMin = 1, UIMin = 1, UIMax = 1500))
	float Rpm = 750;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float ReloadTime;

	UPROPERTY(EditAnywhere)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	UParticleSystem* Impact;

	UPROPERTY(EditAnywhere)
	UAnimMontage* FireAnim;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadAnim;
	
	float FireLag;
	float LastFire;
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(EditAnywhere)
	uint8 bAutomatic : 1;

public:
	uint8 bWantsToFire : 1;
	const AGun* const CDO = nullptr;
};
