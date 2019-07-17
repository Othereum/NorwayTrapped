// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "FpsCharacter.h"
#include "PostureComponent.h"

void AGun::BeginPlay()
{
	Super::BeginPlay();
	const_cast<const AGun*&>(CDO) = GetDefault<AGun>(GetClass());
	if (HasAuthority()) Clip += bChamber;
}

void AGun::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bWantsToFire && State == EWeaponState::Firing)
		State = EWeaponState::Idle;
	else if (bAutomatic && CanFire())
	{
		if (bWantsToFire)
			State = EWeaponState::Firing;
		if (State == EWeaponState::Firing)
			HandleFire(DeltaSeconds);
	}
}

void AGun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (NewMagazineRef) NewMagazineRef->Destroy();
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Clip);
}

void AGun::Deploy()
{
	Super::Deploy();
	OnDeploy();
}

void AGun::Holster(AWeapon* To)
{
	CancelReload();
	SetAiming(false);
	Super::Holster(To);
}

bool AGun::CanFire() const
{
	switch (State)
	{
	case EWeaponState::Idle:
	case EWeaponState::Firing:
	case EWeaponState::Reloading:
		return Clip > 0 && GetWorld()->GetTimeSeconds() - LastFire >= 60.f / Rpm;
	default:
		return false;
	}
}

void AGun::HandleFire(const float DeltaSeconds)
{
	FireLag += DeltaSeconds;
	for (const auto Delay = 60.f / Rpm; FireLag >= Delay; FireLag -= Delay)
	{
		Fire();
	}
}

void AGun::Fire()
{
	CancelReload();
	if (const auto Character = GetCharacter())
		Character->PlayAnimMontage(CharacterFireAnim);
	PlayWepAnim(FireAnim);

	Shoot();

	if (Role != ROLE_SimulatedProxy)
	{
		--Clip;
	}

	OnFire();
}

void AGun::FireP()
{
	const auto Character = GetCharacter();
	if (Character && Character->IsLocallyControlled())
	{
		Character->GetPostureComponent()->Sprint.bPressed = false;
	}
	bWantsToFire = true;
	if (CanFire())
	{
		StartFire();
	}
}

void AGun::FireR()
{
	bWantsToFire = false;
	if (State == EWeaponState::Firing)
	{
		StopFire();
	}
}

void AGun::StartFire()
{
	if (bAutomatic) 
	{
		State = EWeaponState::Firing;
		FireLag = 0.f;
	}
	Fire();
}

void AGun::StopFire()
{
	if (bAutomatic)
	{
		State = EWeaponState::Idle;
		FireLag = 0.f;
	}
}

void AGun::Reload()
{
	if (!CanReload()) return;

	FireR();
	SetAiming(false);

	if (HasAuthority()) State = EWeaponState::Reloading;

	UAnimMontage* Anim;
	float Time;

	const auto bTactical = Clip && bChamber;
	if (bTactical)
	{
		Anim = CharacterTacticalReloadAnim;
		Time = TacticalReloadTime;
	}
	else
	{
		Anim = CharacterFullReloadAnim;
		Time = FullReloadTime;
	}

	PlayOwnerAnim(Anim, Time);
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, [this, bTactical]
	{
		if (HasAuthority())
		{
			Clip = CDO->Clip + bTactical;
			State = EWeaponState::Idle;
		}
	}, Time, false);
}

bool AGun::CanReload() const
{
	switch (State)
	{
	case EWeaponState::Idle:
	case EWeaponState::Firing:
		return Clip < CDO->Clip + bChamber;
	default:
		return false;
	}
}

void AGun::CancelReload()
{
	if (const auto Character = GetCharacter())
	{
		Character->StopAnimMontage(CharacterTacticalReloadAnim);
		Character->StopAnimMontage(CharacterFullReloadAnim);
	}
	StopWepAnim(0.f, MagOutAnim);
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	if (NewMagazineRef) NewMagazineRef->Destroy();
	State = EWeaponState::Idle;
}

void AGun::Shoot()
{
	const auto Character = GetCharacter();
	if (!Character) return;

	auto Start = Character->GetPawnViewLocation();
	FVector End;
	FVector ShotDir;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Character);

	if (bAiming)
	{
		End = Start + (GetMesh()->GetSocketLocation(AimEndSocket) - Start).GetSafeNormal() * MaxRange;
		ShotDir = End - Start;
		ShotDir.Normalize();
	}
	else
	{
		FHitResult CameraHit;
		if (GetWorld()->LineTraceSingleByProfile(CameraHit, Start, Start + Character->GetBaseAimRotation().Vector() * MaxRange, BulletCollisionProfile.Name, QueryParams))
		{
			Start = GetMesh()->GetSocketLocation(MuzzleSocketName);
			ShotDir = CameraHit.Location - Start;
			ShotDir.Normalize();
			End = MaxRange * FMath::VRandCone(ShotDir, HipfireSpread);

			QueryParams.bReturnPhysicalMaterial = true;
		}
	}

	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult BulletHit;
	if (GetWorld()->LineTraceSingleByProfile(BulletHit, Start, End, BulletCollisionProfile.Name, QueryParams))
	{
		HitBullet(BulletHit, ShotDir);
	}
}

void AGun::HitBullet(const FHitResult& Hit, const FVector& ShotDirection)
{
	if (const auto HitActor = Hit.GetActor())
	{
		HitActor->TakeDamage(Damage, FPointDamageEvent{Damage, Hit, ShotDirection, DamageType},
		                     GetCharacter()->Controller, this);
	}

	const auto PhysMat = Hit.PhysMaterial.Get();
	if (PhysMat && Impact.Contains(PhysMat->SurfaceType))
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			Impact[PhysMat->SurfaceType],
			Hit.Location + Hit.Normal,
			Hit.Normal.Rotation(),
			true,
			EPSCPoolMethod::AutoRelease
		);
	}
}

void AGun::AimP()
{
	SetAiming(bToggleToAim ? !bAiming : true);
}

void AGun::AimR()
{
	if (!bToggleToAim)
	{
		SetAiming(false);
	}
}

void AGun::SetAiming(const bool bNewAiming)
{
	if (bNewAiming)
	{
		GetCharacter()->Aim();
		CancelReload();
	}
	else
	{
		GetCharacter()->UnAim();
	}
	bAiming = bNewAiming;
}

FVector AGun::GetAimLocation() const
{
	return GetMesh()->GetSocketLocation(AimSocket);
}

float AGun::GetAimTime() const
{
	return AimTime;
}

float AGun::GetAimFovRatio() const
{
	return IronsightFovRatio;
}

void AGun::DropMag() const
{
	if (!EmptyMagazineClass) return;
	if (const auto DroppedMag = GetWorld()->SpawnActor<AStaticMeshActor>(EmptyMagazineClass, GetMesh()->GetSocketTransform(MagazineSocketName)))
	{
		DroppedMag->GetStaticMeshComponent()->SetStaticMesh(EmptyMagazineMesh);
		if (const auto Character = GetCharacter())
			DroppedMag->GetStaticMeshComponent()->AddImpulse(Character->GetVelocity() * .5f);
	}
}

void AGun::GrabMag() const
{
	const auto Character = GetCharacter();
	if (!Character) return;

	NewMagazineRef = GetWorld()->SpawnActor<AStaticMeshActor>(NewMagazineClass);
	if (NewMagazineRef)
	{
		NewMagazineRef->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, MagazineSocketName);
		NewMagazineRef->GetStaticMeshComponent()->SetStaticMesh(NewMagazineMesh);
	}
}

void AGun::MagIn() const
{
	PlayWepAnim(MagInAnim);
	if (NewMagazineRef)
	{
		NewMagazineRef->Destroy();
	}
}

void AGun::MagOut() const
{
	PlayWepAnim(MagOutAnim);
}

void AGun::Bolt() const
{
	PlayWepAnim(BoltAnim);
}

void AGun::EndReload() const
{
	if (const auto Character = GetCharacter())
		Character->StopAnimMontage(CharacterTacticalReloadAnim);
}
