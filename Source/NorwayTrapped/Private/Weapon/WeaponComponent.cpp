// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "WeaponComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "FpsCharacter.h"
#include "Weapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, Log, All)

UWeaponComponent::UWeaponComponent()
{
	bReplicates = true;
	bWantsInitializeComponent = true;
}

void UWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const_cast<AFpsCharacter*&>(Owner) = CastChecked<AFpsCharacter>(GetOwner());
	Weapons.Init(nullptr, WeaponSlots);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, Weapons);
	DOREPLIFETIME(UWeaponComponent, Active);
}

void UWeaponComponent::SetupPlayerInputComponent(UInputComponent* Input)
{
	Input->BindAction("Fire", IE_Pressed, this, &UWeaponComponent::FireP);
	Input->BindAction("Fire", IE_Released, this, &UWeaponComponent::FireR);
	Input->BindAction("Aim", IE_Pressed, this, &UWeaponComponent::AimP);
	Input->BindAction("Aim", IE_Released, this, &UWeaponComponent::AimR);
	Input->BindAction("Reload", IE_Pressed, this, &UWeaponComponent::Reload);

	FString Slot = TEXT("Slot0");
	for (uint8 i = 0; i < Weapons.Num(); ++i)
	{
		using F = TBaseDelegate<void, uint8>;
		Slot[4] = TEXT('1') + i;
		Input->BindAction<F>(*Slot, IE_Pressed, this, &UWeaponComponent::ServerSetActiveWeapon, i);
	}
}

void UWeaponComponent::SelectWeapon(const uint8 Slot)
{
	Active = Slot;
	Weapons[Slot]->MulticastDeploy();
}

#define DEFINE_ACTION(Name) void UWeaponComponent::Name() { if (auto W = GetActiveWeapon()) { W->Name(); if (!Owner->HasAuthority() && Owner->IsLocallyControlled()) Server##Name(); } } void UWeaponComponent::Server##Name##_Implementation() { Multicast##Name(); } bool UWeaponComponent::Server##Name##_Validate() { return true; } void UWeaponComponent::Multicast##Name##_Implementation() { if (Owner->Role != ROLE_AutonomousProxy) Name(); }
#define DEFINE_PR_ACTION(Name) DEFINE_ACTION(Name##P) DEFINE_ACTION(Name##R)

DEFINE_PR_ACTION(Fire)
DEFINE_PR_ACTION(Aim)
DEFINE_ACTION(Reload)

#undef DEFINE_PR_ACTION
#undef DEFINE_ACTION

void UWeaponComponent::ServerSetActiveWeapon_Implementation(const uint8 Slot)
{
	const auto W = GetActiveWeapon();
	if (Slot < Weapons.Num() && Slot != Active
		&& (!W || W->CanHolster())
		&& Weapons[Slot] && Weapons[Slot]->CanDeploy())
	{
		if (W) W->MulticastHolster(Weapons[Slot]);
		else SelectWeapon(Slot);
	}
}
bool UWeaponComponent::ServerSetActiveWeapon_Validate(const uint8 Slot)
{
	return Slot < Weapons.Max();
}

AWeapon* UWeaponComponent::Give(const TSubclassOf<AWeapon> WeaponClass)
{
	if (!WeaponClass)
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Failed to give weapon: Invalid class"));
		return nullptr;
	}

	const auto Slot = GetDefault<AWeapon>(WeaponClass)->GetSlot();
	if (Slot >= Weapons.Num())
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Failed to give weapon: Invalid slot (Expected: < %d, Actual: %d)"), Weapons.Num(), Slot);
		return nullptr;
	}

	FActorSpawnParameters Parameters;
	Parameters.Owner = Owner;
	Parameters.Instigator = Owner;
	const auto Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Parameters);
	if (!Weapon)
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Failed to give weapon: Can't spawn the weapon"));
		return nullptr;
	}

	Weapon->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	if (Weapons[Slot]) Weapons[Slot]->Destroy();
	Weapons[Slot] = Weapon;
	if (Slot == Active) Weapon->MulticastDeploy();

	return Weapon;
}

void UWeaponComponent::OnRep_Weapons()
{
	const auto ActiveWeapon = GetActiveWeapon();
	if (ActiveWeapon && !ActiveWeapon->IsVisible())
	{
		ActiveWeapon->SetVisibility(true);
	}
}
