// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "CharacterPosture.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FpsCharacter.h"
#include "PostureComponent.h"

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

FCharacterPosture* FStand::Transit(UPostureComponent* Comp) const
{
	const auto NewState = Comp->Crouch.bPressed ? FCrouch::GetObject() : Comp->Prone.bPressed ? FProne::GetObject() : nullptr;
	return NewState && NewState->CanEnter(Comp) ? NewState : nullptr;
}

void FStand::Enter(UPostureComponent* Comp, FCharacterPosture* Before) const
{
	Comp->SetCapsuleHalfHeight(
		Comp->Owner->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

bool FStand::CanEnter(UPostureComponent* Comp) const
{
	return !Comp->IsOverlapped(Comp->Owner->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

void FStand::Exit(UPostureComponent* Comp, FCharacterPosture* After) const
{
	const auto SwitchingTo = After->GetEnum();
	if (SwitchingTo == EPosture::Prone || FMath::IsNearlyZero(Comp->Owner->GetVelocity().Size()))
		Comp->PlayAnimMontage(Comp->Stand.GetSwitchToAnim(SwitchingTo));
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

FCharacterPosture* FCrouch::Transit(UPostureComponent* Comp) const
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

void FCrouch::Enter(UPostureComponent* Comp, FCharacterPosture* Before) const
{
	if (Comp->Prone.bToggle) Comp->Prone.bPressed = false;
	Comp->SetCapsuleHalfHeight(Comp->Crouch.CapsuleHalfHeight, Comp->Crouch.MeshOffset);
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed *= Comp->Crouch.SpeedRatio;
}

bool FCrouch::CanEnter(UPostureComponent* Comp) const
{
	return !Comp->IsSprinting() && !Comp->IsOverlapped(Comp->Crouch.CapsuleHalfHeight);
}

void FCrouch::Exit(UPostureComponent* Comp, FCharacterPosture* After) const
{
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed /= Comp->Crouch.SpeedRatio;
	const auto SwitchingTo = After->GetEnum();
	if (SwitchingTo == EPosture::Prone || FMath::IsNearlyZero(Comp->Owner->GetVelocity().Size()))
		Comp->PlayAnimMontage(Comp->Crouch.GetSwitchToAnim(SwitchingTo));
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

FCharacterPosture* FProne::Transit(UPostureComponent* Comp) const
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

void FProne::Enter(UPostureComponent* Comp, FCharacterPosture* Before) const
{
	if (Comp->Crouch.bToggle) Comp->Crouch.bPressed = false;
	Comp->SetCapsuleHalfHeight(Comp->Prone.CapsuleHalfHeight, Comp->Prone.MeshOffset);
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed *= Comp->Prone.SpeedRatio;
	Comp->SetProneSwitchDelegate();
	Comp->Prone.bSwitching = true;
}

bool FProne::CanEnter(UPostureComponent* Comp) const
{
	return !Comp->IsSprinting();
}

void FProne::Exit(UPostureComponent* Comp, FCharacterPosture* After) const
{
	Comp->Owner->GetCharacterMovement()->MaxWalkSpeed /= Comp->Prone.SpeedRatio;
	const auto Anim = Comp->Prone.GetSwitchToAnim(After->GetEnum());
	Comp->PlayAnimMontage(Anim);
	Comp->SetProneSwitchDelegate();
	Comp->Prone.bSwitching = true;
}
