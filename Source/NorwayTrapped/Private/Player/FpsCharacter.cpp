// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "FpsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PostureComponent.h"
#include "WeaponComponent.h"

//////////////////////////////////////////////////////////////////////////
// AFpsCharacter

AFpsCharacter::AFpsCharacter()
	:Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") },
	Posture{ CreateDefaultSubobject<UPostureComponent>("Posture") },
	Weapon{ CreateDefaultSubobject<UWeaponComponent>("Weapon") }
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
