// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "CharacterPosture.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ChrStateComp.h"

static FStand GStand;
static FCrouch GCrouch;
static FProne GProne;

FCharacterPosture* FCharacterPosture::GetByEnum(const EPosture Enum)
{
	switch (Enum)
	{
	case EPosture::Stand: return FStand::GetObject();
	case EPosture::Crouch: return FCrouch::GetObject();
	case EPosture::Prone: return FProne::GetObject();
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Stand

FCharacterPosture* FStand::GetObject()
{
	return &GStand;
}

EPosture FStand::GetEnum() const
{
	return EPosture::Stand;
}

FCharacterPosture* FStand::Transit(UChrStateComp* Comp) const
{
	const auto NewState = Comp->Crouch.bPressed ? FCrouch::GetObject() : Comp->Prone.bPressed ? FProne::GetObject() : nullptr;
	return NewState && NewState->CanEnter(Comp) ? NewState : nullptr;
}

void FStand::Enter(UChrStateComp* Comp) const
{
	Comp->SetCapsuleHalfHeight(
		Comp->Owner->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

bool FStand::CanEnter(UChrStateComp* Comp) const
{
	return !Comp->IsOverlapped(Comp->Owner->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

//////////////////////////////////////////////////////////////////////////
// Crouch

FCharacterPosture* FCrouch::GetObject()
{
	return &GCrouch;
}

EPosture FCrouch::GetEnum() const
{
	return EPosture::Crouch;
}

FCharacterPosture* FCrouch::Transit(UChrStateComp* Comp) const
{
	const auto NewState = Comp->Prone.bPressed ? FProne::GetObject() : !Comp->Crouch.bPressed ? FStand::GetObject() : nullptr;
	if (!NewState || !NewState->CanEnter(Comp))
	{
		if (Comp->Prone.bPressed && Comp->Prone.bToggle)
		{
			Comp->Prone.bPressed = false;
		}
		else if (!Comp->Crouch.bPressed && Comp->Crouch.bToggle)
		{
			Comp->Crouch.bPressed = true;
		}
		return nullptr;
	}
	return NewState;
}

void FCrouch::Enter(UChrStateComp* Comp) const
{
	if (Comp->Prone.bToggle) Comp->Prone.bPressed = false;
	Comp->SetCapsuleHalfHeight(Comp->Crouch.CapsuleHalfHeight);
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed *= Comp->Crouch.SpeedRatio;
}

bool FCrouch::CanEnter(UChrStateComp* Comp) const
{
	return !Comp->IsSprinting() && !Comp->IsOverlapped(Comp->Crouch.CapsuleHalfHeight);
}

void FCrouch::Exit(UChrStateComp* Comp) const
{
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed /= Comp->Crouch.SpeedRatio;
}

//////////////////////////////////////////////////////////////////////////
// Prone

FCharacterPosture* FProne::GetObject()
{
	return &GProne;
}

EPosture FProne::GetEnum() const
{
	return EPosture::Prone;
}

FCharacterPosture* FProne::Transit(UChrStateComp* Comp) const
{
	if (Comp->Crouch.bPressed)
	{
		const auto Crouch = FCrouch::GetObject();
		if (Crouch->CanEnter(Comp)) return Crouch;
	}
	else if (!Comp->Prone.bPressed)
	{
		const auto Stand = FStand::GetObject();
		if (Stand->CanEnter(Comp)) return Stand;

		const auto Crouch = FCrouch::GetObject();
		if (Crouch->CanEnter(Comp)) return Crouch;
	}

	if (Comp->Crouch.bPressed && Comp->Crouch.bToggle)
	{
		Comp->Crouch.bPressed = false;
	}
	else if (!Comp->Prone.bPressed && Comp->Prone.bToggle)
	{
		Comp->Prone.bPressed = true;
	}

	return nullptr;
}

void FProne::Enter(UChrStateComp* Comp) const
{
	if (Comp->Crouch.bToggle) Comp->Crouch.bPressed = false;
	Comp->SetCapsuleHalfHeight(Comp->Prone.CapsuleHalfHeight);
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed *= Comp->Prone.SpeedRatio;
}

bool FProne::CanEnter(UChrStateComp* Comp) const
{
	return !Comp->IsSprinting();
}

void FProne::Exit(UChrStateComp* Comp) const
{
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed /= Comp->Prone.SpeedRatio;
}
