// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "NorwayTrappedCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All)

//////////////////////////////////////////////////////////////////////////
// ANorwayTrappedCharacter

ANorwayTrappedCharacter::ANorwayTrappedCharacter()
	:Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") }
{
	PrimaryActorTick.bCanEverTick = true;
	Camera->SetupAttachment(GetMesh(), "Eye");
}

void ANorwayTrappedCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ANorwayTrappedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		if (!bSprinting && CanSprint())
		{
			SetSprinting(true);
			ServerSetSprinting(true);
		}
		else if (bSprinting && !CanSprint())
		{
			SetSprinting(false);
			ServerSetSprinting(false);
		}
	}
}

void ANorwayTrappedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANorwayTrappedCharacter, bSprinting);
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

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ANorwayTrappedCharacter::WalkPressed);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ANorwayTrappedCharacter::WalkReleased);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ANorwayTrappedCharacter::SprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ANorwayTrappedCharacter::SprintReleased);
}

void ANorwayTrappedCharacter::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);

		// Only for keyboard
		if (CanWalk()) Walk();
	}
}

void ANorwayTrappedCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);

		// Only for keyboard
		if (CanWalk()) Walk();
	}
}

void ANorwayTrappedCharacter::Walk()
{
	ControlInputVector.Normalize();
	ControlInputVector *= .5f;
}

bool ANorwayTrappedCharacter::CanWalk() const
{
	return bWantsToWalk && !bSprinting;
}

void ANorwayTrappedCharacter::WalkPressed()
{
	bWantsToWalk = true;
}

void ANorwayTrappedCharacter::WalkReleased()
{
	bWantsToWalk = false;
}

bool ANorwayTrappedCharacter::CanSprint() const
{
	return bWantsToSprint && GetInputAxisValue("MoveForward") > 0.f;
}

void ANorwayTrappedCharacter::SprintPressed()
{
	bWantsToSprint = true;
}

void ANorwayTrappedCharacter::SprintReleased()
{
	bWantsToSprint = false;
}

void ANorwayTrappedCharacter::SetSprinting(bool b)
{
	GetCharacterMovement()->MaxWalkSpeed = b ? MaxSprintSpeed : GetClass()->GetDefaultObject<ANorwayTrappedCharacter>()->GetCharacterMovement()->MaxWalkSpeed;
	bSprinting = b;
}

void ANorwayTrappedCharacter::ServerSetSprinting_Implementation(bool b)
{
	SetSprinting(b);
}

bool ANorwayTrappedCharacter::ServerSetSprinting_Validate(bool)
{
	return true;
}
