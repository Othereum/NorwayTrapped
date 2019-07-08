// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "WeaponComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "UnrealNetwork.h"
#include "Weapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, Log, All)

UWeaponComponent::UWeaponComponent()
{
	bReplicates = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const_cast<ACharacter*&>(Owner) = CastChecked<ACharacter>(GetOwner());
	Weapons.Init(nullptr, WeaponSlots);
}

void UWeaponComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, Weapons);
	DOREPLIFETIME(UWeaponComponent, Active);
}

void UWeaponComponent::SetupPlayerInputComponent(UInputComponent* Input)
{
	TCHAR* Slot = TEXT("Slot0");
	for (uint8 i = 0; i < Weapons.Num(); ++i)
	{
		using F = TBaseDelegate<void, uint8>;
		Slot[4] = TEXT('1') + i;
		Input->BindAction<F>(Slot, IE_Pressed, this, &UWeaponComponent::SetActiveWeapon, i);
	}
}

void UWeaponComponent::SetActiveWeapon(const uint8 Slot)
{
	if (Slot < Weapons.Num()
		&& (!Weapons[Active] || Weapons[Active]->Holster(Weapons[Slot]))
		&& Weapons[Slot] && Weapons[Slot]->Deploy())
	{
		Active = Slot;
	}
}

AWeapon* UWeaponComponent::Give(const TSubclassOf<AWeapon> WeaponClass)
{
	if (!WeaponClass) return nullptr;

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
	if (!Weapon) return nullptr;

	if (Weapons[Slot]) Weapons[Slot]->Destroy();
	Weapons[Slot] = Weapon;
	if (Slot == Active) Weapon->Deploy();

	Weapon->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);

	return Weapon;
}

void UWeaponComponent::OnRep_Active(const uint8 Old)
{
	if (Weapons[Old]) Weapons[Old]->Holster(Weapons[Active]);
	if (Weapons[Active]) Weapons[Active]->Deploy();
}
