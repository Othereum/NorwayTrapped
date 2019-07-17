// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CollisionProfile.h"
#include "Weapon/Weapon.h"
#include "Gun.generated.h"

class AStaticMeshActor;

UCLASS()
class AGun : public AWeapon
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnFire();

	void DropMag() const;
	void GrabMag() const;
	void MagIn() const;
	void MagOut() const;
	void Bolt() const;
	void EndReload() const;

	uint8 GetClip() const { return Clip; }
	bool CanFire() const;
	bool CanReload() const;
	void CancelReload();

	bool IsAiming() const { return bAiming; }
	void SetAiming(bool bNewAiming);

	// Returns the world location of the camera when aiming.
	virtual FVector GetAimLocation() const;
	virtual float GetAimTime() const;
	virtual float GetAimFovRatio() const;
	virtual float GetHipfireSpread() const;

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Holster(AWeapon* To) override;
	void AimP() override;
	void AimR() override;

private:
	void HandleFire(float DeltaSeconds);
	void Fire();
	void FireP() override;
	void FireR() override;
	void StartFire();
	void StopFire();
	void Reload() override;
	void Shoot();
	void HitBullet(const FHitResult& Hit, const FVector& ShotDirection);

	UFUNCTION()
	void OnRep_HipfireRandSeed();

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Clip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=1, UIMin=1, UIMax=1500))
	float Rpm = 750;

	UPROPERTY(EditAnywhere, meta=(UIMin=0, UIMax=1))
	float HipfireSpread = .3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=0.1, UIMin=1, EditCondition=bChamber))
	float TacticalReloadTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=0.1, UIMin=1))
	float FullReloadTime = 1;

	UPROPERTY(EditAnywhere)
	float AimTime = .4f;

	UPROPERTY(EditAnywhere)
	float IronsightFovRatio = .9f;

	UPROPERTY(EditAnywhere)
	FVector AimOffset;

	UPROPERTY(EditAnywhere, Category=Socket)
	FName AimSocket = "Aim";

	UPROPERTY(EditAnywhere, Category=Socket)
	FName AimEndSocket = "AimEnd";

	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=0, UIMin=0))
	float MaxRange = 50000;

	UPROPERTY(EditAnywhere, Category=Socket)
	FName MuzzleSocketName = "Muzzle";

	UPROPERTY(EditAnywhere)
	FCollisionProfileName BulletCollisionProfile;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> Impact;

	UPROPERTY(EditAnywhere)
	UParticleSystem* Trail;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* CharacterFireAnim;
	
	UPROPERTY(EditAnywhere, Category=Animation, meta=(EditCondition=bChamber))
	UAnimMontage* CharacterTacticalReloadAnim;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* CharacterFullReloadAnim;
	
	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* FireAnim;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* MagOutAnim;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* MagInAnim;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* BoltAnim;

	UPROPERTY(EditAnywhere, Category=Socket)
	FName MagazineSocketName = "Magazine";

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStaticMeshActor> NewMagazineClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStaticMeshActor> EmptyMagazineClass;

	UPROPERTY(EditAnywhere)
	UStaticMesh* NewMagazineMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* EmptyMagazineMesh;

	UPROPERTY(Transient)
	mutable AStaticMeshActor* NewMagazineRef;

	float FireLag;
	float LastFire;
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(EditAnywhere)
	uint8 bAutomatic : 1;

	UPROPERTY(EditAnywhere)
	uint8 bChamber : 1;

	UPROPERTY(EditAnywhere)
	uint8 bToggleToAim : 1;

	UPROPERTY(VisibleInstanceOnly, Replicated, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bAiming : 1;

	UPROPERTY(ReplicatedUsing=OnRep_HipfireRandSeed, Transient)
	int32 HipfireRandSeed;
	FRandomStream HipfireSpreadRand;

public:
	uint8 bWantsToFire : 1;
	const AGun* const CDO = nullptr;
};
