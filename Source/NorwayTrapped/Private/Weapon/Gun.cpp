// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Gun.h"

void AGun::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bAutomatic && State == EWeaponState::Firing)
	{
		HandleFire(DeltaSeconds);
	}
}

bool AGun::CanFire() const
{
	switch (State)
	{
	case EWeaponState::Idle:
	case EWeaponState::Firing:
	case EWeaponState::Reloading:
		return true;
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
	UE_LOG(LogClass, Log, TEXT("Fire!"));
}

void AGun::FireP()
{
	State = EWeaponState::Firing;
	FireLag = 0.f;
	Fire();
}

void AGun::FireR()
{
	State = EWeaponState::Idle;
	FireLag = 0.f;
}
