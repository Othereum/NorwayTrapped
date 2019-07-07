// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "WeaponComponent.h"
#include "GameFramework/Character.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	const_cast<ACharacter*&>(Owner) = CastChecked<ACharacter>(GetOwner());
}

void UWeaponComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
