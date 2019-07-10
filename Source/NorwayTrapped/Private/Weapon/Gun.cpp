// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Gun.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "FpsCharacter.h"
#include "PostureComponent.h"

void AGun::BeginPlay()
{
	Super::BeginPlay();
	const_cast<const AGun*&>(CDO) = GetDefault<AGun>(GetClass());
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
	// Cancel reload
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	Owner->PlayAnimMontage(FireAnim);
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GetMesh(), "Muzzle", FVector::ZeroVector, FRotator::ZeroRotator,
	                                       EAttachLocation::SnapToTarget, true, EPSCPoolMethod::AutoRelease);
	Shoot();
	if (Role != ROLE_SimulatedProxy)
	{
		--Clip;
	}
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
	if (HasAuthority()) State = EWeaponState::Reloading;
	PlayAnim(ReloadAnim, ReloadTime);
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, [this]
	{
		if (HasAuthority())
		{
			Clip = CDO->Clip;
			State = EWeaponState::Idle;
		}
	}, ReloadTime, false);
}

void AGun::Shoot()
{
	const auto Start = GetMesh()->GetSocketLocation("Muzzle");
	const auto End = Owner->GetCamera()->GetComponentLocation() + Owner->GetBaseAimRotation().Vector() * 100000.f;

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, "Projectile"))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, Impact, HitResult.Location + HitResult.Normal, HitResult.Normal.Rotation(),
			true, EPSCPoolMethod::AutoRelease);
	}
}
