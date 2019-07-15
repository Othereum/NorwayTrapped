// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Gun.h"
#include "Camera/CameraComponent.h"
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

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Clip);
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
	Owner->PlayAnimMontage(FireAnim3P);
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
	if (Owner->IsLocallyControlled())
	{
		Owner->GetPosture()->Sprint.bPressed = false;
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
	if (HasAuthority()) State = EWeaponState::Reloading;

	UAnimMontage* Anim;
	float Time;

	const auto bTactical = Clip && bChamber;
	if (bTactical)
	{
		Anim = TacticalReloadAnim3P;
		Time = TacticalReloadTime;
	}
	else
	{
		Anim = FullReloadAnim3P;
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
	GetMesh()->Stop();
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	if (MagazineRef) MagazineRef->Destroy();
}

void AGun::Shoot()
{
	const auto CameraLocation = Owner->GetCamera()->GetComponentLocation();
	const auto CameraEnd = CameraLocation + Owner->GetBaseAimRotation().Vector() * MaxRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Owner);

	FHitResult CameraHit;
	if (GetWorld()->LineTraceSingleByProfile(CameraHit, CameraLocation, CameraEnd, BulletCollisionProfile.Name, QueryParams))
	{
		const auto MuzzleLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);

		QueryParams.bReturnPhysicalMaterial = true;

		auto ShotDir = CameraHit.Location - MuzzleLocation;
		ShotDir.Normalize();

		FHitResult BulletHit;
		if (GetWorld()->LineTraceSingleByProfile(BulletHit, MuzzleLocation, CameraHit.Location + ShotDir, BulletCollisionProfile.Name, QueryParams))
		{
			if (const auto HitActor = BulletHit.GetActor())
			{
				HitActor->TakeDamage(Damage, FPointDamageEvent{
					Damage, BulletHit, ShotDir, DamageType
				}, Owner->Controller, this);
			}

			const auto PhysMat = BulletHit.PhysMaterial.Get();
			if (PhysMat && Impact.Contains(PhysMat->SurfaceType))
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, Impact[PhysMat->SurfaceType],
				                                         BulletHit.Location + BulletHit.Normal,
				                                         BulletHit.Normal.Rotation(), true,
				                                         EPSCPoolMethod::AutoRelease);
			}
		}
	}
}

void AGun::DropMag() const
{
	if (!EmptyMagazineClass) return;
	if (const auto DroppedMag = GetWorld()->SpawnActor<AStaticMeshActor>(EmptyMagazineClass, GetMesh()->GetSocketTransform(MagazineSocketName)))
	{
		DroppedMag->GetStaticMeshComponent()->SetStaticMesh(EmptyMagazineMesh);
		DroppedMag->GetStaticMeshComponent()->AddImpulse(Owner->GetVelocity() * .5f);
	}
}

void AGun::GrabMag() const
{
	MagazineRef = GetWorld()->SpawnActor<AStaticMeshActor>(MagazineClass);
	if (MagazineRef)
	{
		MagazineRef->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, MagazineSocketName);
		MagazineRef->GetStaticMeshComponent()->SetStaticMesh(MagazineMesh);
	}
}

void AGun::MagIn() const
{
	PlayWepAnim(MagInAnim);
	if (MagazineRef)
	{
		MagazineRef->Destroy();
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
