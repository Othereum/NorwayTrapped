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

AFpsCharacter::AFpsCharacter()
	:Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") },
	Posture{ CreateDefaultSubobject<UPostureComponent>("Posture") },
	Weapon{ CreateDefaultSubobject<UWeaponComponent>("Weapon") },
	bAlive{ true }
{
	Camera->SetupAttachment(GetMesh(), "Eye");
	Weapon->SetupAttachment(GetMesh(), "RightHand");
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

	Posture->SetupPlayerInputComponent(Input);
	Weapon->SetupPlayerInputComponent(Input);
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
}

float AFpsCharacter::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Hp -= Damage;
	if (Hp <= 0.f) Kill();
	return Damage;
}

bool AFpsCharacter::ShouldTakeDamage(float, const FDamageEvent&, AController*, AActor*) const
{
	return bAlive;
}
