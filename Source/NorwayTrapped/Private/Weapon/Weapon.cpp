// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"

AWeapon::AWeapon()
	:Mesh{ CreateDefaultSubobject<USkeletalMeshComponent>("Mesh") }
{
	RootComponent = Mesh;
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}
