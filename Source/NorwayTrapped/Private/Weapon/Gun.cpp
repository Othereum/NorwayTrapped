// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Gun.h"
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
	State = EWeaponState::Firing;
	FireLag = 0.f;
	Fire();
}

void AGun::StopFire()
{
	State = EWeaponState::Idle;
	FireLag = 0.f;
}

void AGun::Reload()
{
	Clip = CDO->Clip;
}
