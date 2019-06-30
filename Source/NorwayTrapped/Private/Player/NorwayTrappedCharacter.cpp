// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "NorwayTrappedCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"

#include "ChrStateComp.h"

//////////////////////////////////////////////////////////////////////////
// ANorwayTrappedCharacter

ANorwayTrappedCharacter::ANorwayTrappedCharacter()
	:Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") },
	State{ CreateDefaultSubobject<UChrStateComp>("State") }
{
	PrimaryActorTick.bCanEverTick = true;
	Camera->SetupAttachment(GetMesh(), "Eye");
}

void ANorwayTrappedCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ANorwayTrappedCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANorwayTrappedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

//////////////////////////////////////////////////////////////////////////
// Input

void ANorwayTrappedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANorwayTrappedCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANorwayTrappedCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	State->SetupPlayerInputComponent(PlayerInputComponent);
}

void ANorwayTrappedCharacter::MoveForward(const float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANorwayTrappedCharacter::MoveRight(const float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}
