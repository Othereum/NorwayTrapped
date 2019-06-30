// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "CharacterPosture.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
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

void FStand::Transit(UChrStateComp* Comp, FCharacterPosture*& State, EPosture& Enum)
{
	const auto NewState = Comp->Crouch.bPressed ? FCrouch::GetObject() : Comp->Prone.bPressed ? FProne::GetObject() : nullptr;
	if (NewState && NewState->CanEnter(Comp))
	{
		NewState->Enter(Comp);
		State = NewState;
		Enum = NewState->GetEnum();
	}
}

void FStand::Enter(UChrStateComp* Comp)
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

void FCrouch::Transit(UChrStateComp* Comp, FCharacterPosture*& State, EPosture& Enum)
{
	const auto NewState = Comp->Prone.bPressed ? FProne::GetObject() : !Comp->Crouch.bPressed ? FStand::GetObject() : nullptr;
	if (NewState && NewState->CanEnter(Comp))
	{
		NewState->Enter(Comp);
		State = NewState;
		Enum = NewState->GetEnum();
	}
}

void FCrouch::Enter(UChrStateComp* Comp)
{
	Comp->Prone.bPressed = false;
	Comp->SetCapsuleHalfHeight(Comp->Crouch.CapsuleHalfHeight);
}

bool FCrouch::CanEnter(UChrStateComp* Comp) const
{
	return !Comp->IsOverlapped(Comp->Crouch.CapsuleHalfHeight);
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

void FProne::Transit(UChrStateComp* Comp, FCharacterPosture*& State, EPosture& Enum)
{
	const auto NewState = Comp->Crouch.bPressed ? FCrouch::GetObject() : !Comp->Prone.bPressed ? FStand::GetObject() : nullptr;
	if (NewState && NewState->CanEnter(Comp))
	{
		NewState->Enter(Comp);
		State = NewState;
		Enum = NewState->GetEnum();
	}
}

void FProne::Enter(UChrStateComp* Comp)
{
	Comp->Crouch.bPressed = false;
	Comp->SetCapsuleHalfHeight(Comp->Prone.CapsuleHalfHeight);
}
