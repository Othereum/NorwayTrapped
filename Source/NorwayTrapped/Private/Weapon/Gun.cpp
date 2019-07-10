// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"

void AGun::BeginPlay()
{
	Super::BeginPlay();
	const_cast<const AGun*&>(CDO) = GetDefault<AGun>(GetClass());
}

void AGun::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bAutomatic && CanFire())
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
		return Clip > 0;
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
	--Clip;
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GetMesh(), "Muzzle", FVector::ZeroVector, FRotator::ZeroRotator,
	                                       EAttachLocation::SnapToTarget, true, EPSCPoolMethod::AutoRelease);
	OnFire();
}

void AGun::FireP()
{
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
		Fire();
	}
	else
	{
		const auto CurTime = GetGameTimeSinceCreation();
		auto& LastFireTime = FireLag;
		if (CurTime - LastFireTime >= 60.f / Rpm)
		{
			LastFireTime = CurTime;
			Fire();
		}
	}
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
	Clip = CDO->Clip;
}
