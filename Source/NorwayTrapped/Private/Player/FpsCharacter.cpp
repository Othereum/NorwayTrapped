// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "FpsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"
#include "PostureComponent.h"
#include "WeaponComponent.h"

//////////////////////////////////////////////////////////////////////////
// AFpsCharacter

const FName AFpsCharacter::CameraComponentName = "CameraComponent";
const FName AFpsCharacter::PostureComponentName = "PostureComponent";
const FName AFpsCharacter::WeaponComponentName = "WeaponComponent";

AFpsCharacter::AFpsCharacter()
	:CameraComponent{ CreateDefaultSubobject<UCameraComponent>(CameraComponentName) },
	PostureComponent{ CreateDefaultSubobject<UPostureComponent>(PostureComponentName) },
	WeaponComponent{ CreateDefaultSubobject<UWeaponComponent>(WeaponComponentName) },
	bAlive{ true }
{
	GetMesh()->bReturnMaterialOnMove = true;
	CameraComponent->SetupAttachment(GetMesh(), "Eye");
	WeaponComponent->SetupAttachment(GetMesh(), "RightHand");
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFpsCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	Input->BindAxis("MoveForward", this, &AFpsCharacter::MoveForward);
	Input->BindAxis("MoveRight", this, &AFpsCharacter::MoveRight);

	Input->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	Input->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PostureComponent->SetupPlayerInputComponent(Input);
	WeaponComponent->SetupPlayerInputComponent(Input);
}

void AFpsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFpsCharacter, Hp);
	DOREPLIFETIME(AFpsCharacter, bAlive);
}

void AFpsCharacter::MoveForward(const float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFpsCharacter::MoveRight(const float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

//////////////////////////////////////////////////////////////////////////
// HP / Damage

void AFpsCharacter::Kill()
{
	Hp = 0.f;
	bAlive = false;
	OnKill();
}

void AFpsCharacter::MulticastKill_Implementation()
{
	Kill();
}

float AFpsCharacter::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage > 0.f)
	{
		FHitResult Hit;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, DamageCauser, Hit, ImpulseDir);
		if (Hit.BoneName != NAME_None && HitBoneDmgMul.Contains(Hit.BoneName))
			Damage *= HitBoneDmgMul[Hit.BoneName];

		Hp -= Damage;
		if (Hp <= 0.f) MulticastKill();
	}
	return Damage;
}

bool AFpsCharacter::ShouldTakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	return bAlive && Super::ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
