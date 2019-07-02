// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "ChrStateComp.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnrealNetwork.h"
#include "CharacterPosture.h"

void FStateInputData::Press(UChrStateComp* Comp)
{
	bPressed = bToggle ? !bPressed : true;
}

void FStateInputData::Release(UChrStateComp* Comp)
{
	if (!bToggle) bPressed = false;
}

void FPostureData::Press(UChrStateComp* Comp)
{
	bPressed = bToggle ? (Comp->IsSprinting() ? bPressed : !bPressed) : true;
}

UChrStateComp::UChrStateComp()
{
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UChrStateComp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(MoveForwardInputAxisName, this, &UChrStateComp::WalkIfCan);
	PlayerInputComponent->BindAction(Walk.InputName, IE_Pressed, this, &UChrStateComp::WalkPressed);
	PlayerInputComponent->BindAction(Walk.InputName, IE_Released, this, &UChrStateComp::WalkReleased);
	PlayerInputComponent->BindAction(Sprint.InputName, IE_Pressed, this, &UChrStateComp::SprintPressed);
	PlayerInputComponent->BindAction(Sprint.InputName, IE_Released, this, &UChrStateComp::SprintReleased);
	PlayerInputComponent->BindAction(Crouch.InputName, IE_Pressed, this, &UChrStateComp::CrouchPressed);
	PlayerInputComponent->BindAction(Crouch.InputName, IE_Released, this, &UChrStateComp::CrouchReleased);
	PlayerInputComponent->BindAction(Prone.InputName, IE_Pressed, this, &UChrStateComp::PronePressed);
	PlayerInputComponent->BindAction(Prone.InputName, IE_Released, this, &UChrStateComp::ProneReleased);
}

void UChrStateComp::WalkIfCan(float)
{
	if (Walk.bPressed && !bSprinting)
	{
		auto ControlInputVector = Owner->ConsumeMovementInputVector();
		ControlInputVector.Normalize();
		ControlInputVector *= .5f;
		Owner->AddMovementInput(ControlInputVector, 1.f, true);
	}
}

void UChrStateComp::OnRep_Posture()
{
	if (PostureState->GetEnum() != Posture)
	{
		PostureState->Exit(this);
		PostureState = FCharacterPosture::GetByEnum(Posture);
		PostureState->Enter(this);
	}
}

void UChrStateComp::ServerSetPosture_Implementation(const EPosture NewPosture)
{
	if (Posture != NewPosture)
	{
		PostureState->Exit(this);
		PostureState = FCharacterPosture::GetByEnum(NewPosture);
		Posture = NewPosture;
		PostureState->Enter(this);
	}
}

bool UChrStateComp::ServerSetPosture_Validate(const EPosture NewPosture)
{
	switch (NewPosture)
	{
	case EPosture::Stand:
	case EPosture::Crouch:
	case EPosture::Prone:
		return true;
	}
	return false;
}

bool UChrStateComp::CanSprint() const
{
	return Sprint.bPressed && Owner->GetInputAxisValue("MoveForward") > 0.f;
}

void UChrStateComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UChrStateComp, bSprinting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UChrStateComp, Posture, COND_SkipOwner);
}

void UChrStateComp::BeginPlay()
{
	Super::BeginPlay();

	const_cast<ACharacter*&>(Owner) = CastChecked<ACharacter>(GetOwner());
	PostureState = FCharacterPosture::GetByEnum(Posture);
}

void UChrStateComp::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Owner->IsLocallyControlled())
	{
		if (!bSprinting && CanSprint())
		{
			TrySetSprintingAndTransit(true);
		}
		else if (bSprinting && !CanSprint())
		{
			TrySetSprintingAndTransit(false);
		}
		else
		{
			Transit();
		}
	}
}

void UChrStateComp::WalkPressed()
{
	Walk.Press(this);
}

void UChrStateComp::WalkReleased()
{
	Walk.Release(this);
}

void UChrStateComp::SprintPressed()
{
	Sprint.Press(this);
}

void UChrStateComp::SprintReleased()
{
	Sprint.Release(this);
}

void UChrStateComp::CrouchPressed()
{
	Crouch.Press(this);
}

void UChrStateComp::CrouchReleased()
{
	Crouch.Release(this);
}

void UChrStateComp::PronePressed()
{
	Prone.Press(this);
}

void UChrStateComp::ProneReleased()
{
	Prone.Release(this);
}

void UChrStateComp::ServerSetSprinting_Implementation(const bool b)
{
	SetSprinting_Internal(b);
}

bool UChrStateComp::ServerSetSprinting_Validate(bool)
{
	return true;
}

void UChrStateComp::TrySetSprintingAndTransit(const bool b)
{
	if (b)
	{
		if (Crouch.bToggle) Crouch.bPressed = false;
		if (Prone.bToggle) Prone.bPressed = false;
		if (Walk.bToggle) Walk.bPressed = false;
	}
	Transit();
	if (!b || Posture == EPosture::Stand)
	{
		SetSprinting_Internal(b);
		if (Owner->Role == ROLE_AutonomousProxy)
		{
			ServerSetSprinting(b);
		}
	}
}

void UChrStateComp::SetSprinting_Internal(const bool b)
{
	if (bSprinting != b)
	{
		if (b) Owner->GetCharacterMovement()->MaxWalkSpeed *= Sprint.SpeedRatio;
		else Owner->GetCharacterMovement()->MaxWalkSpeed /= Sprint.SpeedRatio;
	}
	bSprinting = b;
}

void UChrStateComp::SetCapsuleHalfHeight(const float Height) const
{
	const auto MeshOffset = -GetDefault<ACharacter>(Owner->GetClass())->GetMesh()->RelativeLocation.Z - Height;
	Owner->OnStartCrouch(MeshOffset, MeshOffset * Owner->GetActorScale().Z);
	Owner->GetMesh()->UpdateComponentToWorld();

	if (Owner->Role != ROLE_SimulatedProxy)
	{
		Owner->AddActorLocalOffset({ 0.f, 0.f, (Height - Owner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()) * Owner->GetActorScale().Z });
	}
	Owner->GetCapsuleComponent()->SetCapsuleHalfHeight(Height);
}

bool UChrStateComp::IsOverlapped(const float Height) const
{
	auto Pos = Owner->GetActorLocation();
	Pos.Z += Height - Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const auto Capsule = FCollisionShape::MakeCapsule(Owner->GetCapsuleComponent()->GetScaledCapsuleRadius(), Height);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	return GetWorld()->OverlapBlockingTestByProfile(Pos, FQuat::Identity, "Pawn", Capsule, Params);
}

void UChrStateComp::Transit()
{
	const auto NewState = PostureState->Transit(this);
	if (NewState)
	{
		PostureState->Exit(this);
		PostureState = NewState;
		Posture = NewState->GetEnum();
		NewState->Enter(this);
		if (Owner->Role == ROLE_AutonomousProxy)
		{
			ServerSetPosture(Posture);
		}
	}
}
