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

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Holster(AWeapon* To) override;

	UFUNCTION(BlueprintImplementableEvent)
	void AimP() override;
	UFUNCTION(BlueprintImplementableEvent)
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

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Clip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=1, UIMin=1, UIMax=1500))
	float Rpm = 750;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=0.1, UIMin=1, EditCondition=bChamber))
	float TacticalReloadTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=0.1, UIMin=1))
	float FullReloadTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ClampMin=0, UIMin=0))
	float MaxRange = 50000;

	UPROPERTY(EditAnywhere)
	FName MuzzleSocketName = "Muzzle";

	UPROPERTY(EditAnywhere)
	FCollisionProfileName BulletCollisionProfile;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> Impact;

	UPROPERTY(EditAnywhere)
	UAnimMontage* FireAnim3P;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition=bChamber))
	UAnimMontage* TacticalReloadAnim3P;

	UPROPERTY(EditAnywhere)
	UAnimMontage* FullReloadAnim3P;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* FireAnim;

	UPROPERTY(EditAnywhere)
	UAnimMontage* MagOutAnim;

	UPROPERTY(EditAnywhere)
	UAnimMontage* MagInAnim;

	UPROPERTY(EditAnywhere)
	UAnimMontage* BoltAnim;

	UPROPERTY(EditAnywhere)
	FName MagazineSocketName = "Magazine";

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStaticMeshActor> MagazineClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStaticMeshActor> EmptyMagazineClass;

	UPROPERTY(EditAnywhere)
	UStaticMesh* MagazineMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* EmptyMagazineMesh;

	UPROPERTY(Transient)
	mutable AStaticMeshActor* MagazineRef;

	float FireLag;
	float LastFire;
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(EditAnywhere)
	uint8 bAutomatic : 1;

	UPROPERTY(EditAnywhere)
	uint8 bChamber : 1;

public:
	uint8 bWantsToFire : 1;
	const AGun* const CDO = nullptr;
};
