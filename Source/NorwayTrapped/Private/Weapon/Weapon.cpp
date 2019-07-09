// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "FpsCharacter.h"
#include "WeaponComponent.h"

AWeapon::AWeapon()
	:Mesh{ CreateDefaultSubobject<USkeletalMeshComponent>("Mesh") }
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = Mesh;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	const_cast<AFpsCharacter*&>(Owner) = CastChecked<AFpsCharacter>(GetOwner());
	Role = Owner->Role;
	CopyRemoteRoleFrom(Owner);

	if (!HasAuthority() && State == EWeaponState::NeverDeployed && Owner->GetWeapon()->GetActiveWeapon() == this)
	{
		Deploy();
	}
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Mesh->SetVisibility(false);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, State);
}

void AWeapon::MulticastDeploy_Implementation()
{
	Deploy();
}

void AWeapon::MulticastHolster_Implementation(AWeapon* To)
{
	Holster(To);
}

bool AWeapon::IsVisible() const
{
	return Mesh->bVisible;
}

void AWeapon::SetVisibility(const bool bNewVisibility) const
{
	Mesh->SetVisibility(bNewVisibility);
}

void AWeapon::Deploy()
{
	Mesh->SetVisibility(true);
	if (HasAuthority())
	{
		State = EWeaponState::Deploying;
		GetWorldTimerManager().SetTimer(StateSetTimer, [this] { State = EWeaponState::Idle; }, DeployTime, false);
	}
}

bool AWeapon::CanDeploy() const
{
	switch (State)
	{
	case EWeaponState::NeverDeployed:
	case EWeaponState::Unequipped:
		return true;
	default:
		return false;
	}
}

void AWeapon::Holster(AWeapon* To)
{
	if (HasAuthority()) State = EWeaponState::Holstering;
	const auto ToSlot = To->GetSlot();
	GetWorldTimerManager().SetTimer(StateSetTimer, [this, ToSlot]
	{
		Mesh->SetVisibility(false);
		if (HasAuthority())
		{
			State = EWeaponState::Unequipped;
			Owner->GetWeapon()->SelectWeapon(ToSlot);
		}
	}, HolsterTime, false);
}

bool AWeapon::CanHolster() const
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
