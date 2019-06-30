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
	Comp->Transit();
}

void FStateInputData::Release(UChrStateComp* Comp)
{
	if (!bToggle)
	{
		bPressed = false;
		Comp->Transit();
	}
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

bool UChrStateComp::CanSprint() const
{
	return Sprint.bPressed && Owner->GetInputAxisValue("MoveForward") > 0.f;
}

void UChrStateComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UChrStateComp, bSprinting);
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
			SetSprinting(true);
		}
		else if (bSprinting && !CanSprint())
		{
			SetSprinting(false);
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
	Walk.bPressed = false;
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

void UChrStateComp::SetSprinting(const bool b)
{
	SetSprinting_Internal(b);
	if (IsNetSimulating()) ServerSetSprinting(b);
}

void UChrStateComp::SetSprinting_Internal(const bool b)
{
	auto Speed = Owner->GetClass()->GetDefaultObject<ACharacter>()->GetCharacterMovement()->MaxWalkSpeed;
	if (b) Speed *= Sprint.SpeedRatio;
	Owner->GetCharacterMovement()->MaxWalkSpeed = Speed;
	bSprinting = b;
}

void UChrStateComp::SetCapsuleHalfHeight(const float Height) const
{
	const auto Offset = Height - Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	Owner->AddActorLocalOffset({ 0.f, 0.f, Offset });
	Owner->GetMesh()->AddRelativeLocation({ 0.f, 0.f, -Offset });
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
	PostureState->Transit(this, PostureState, Posture);
}
