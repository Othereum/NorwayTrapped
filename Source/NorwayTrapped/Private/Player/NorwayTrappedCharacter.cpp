// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "NorwayTrappedCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"

#include "CharacterStateComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All)

//////////////////////////////////////////////////////////////////////////
// ANorwayTrappedCharacter

ANorwayTrappedCharacter::ANorwayTrappedCharacter()
	:Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") },
	State{ CreateDefaultSubobject<UCharacterStateComponent>("State") }
{
	Camera->SetupAttachment(GetMesh(), "Eye");
}

void ANorwayTrappedCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ANorwayTrappedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANorwayTrappedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveForward", this, &ANorwayTrappedCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANorwayTrappedCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	State->SetupPlayerInputComponent(PlayerInputComponent);
}

void ANorwayTrappedCharacter::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANorwayTrappedCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}
