// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "PostureComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnrealNetwork.h"
#include "CharacterPosture.h"
#include "Animation/AnimInstance.h"

void FStateInputData::Press(UPostureComponent* Comp)
{
	bPressed = bToggle ? !bPressed : true;
}

void FStateInputData::Release(UPostureComponent* Comp)
{
	if (!bToggle) bPressed = false;
}

void FPostureData::Press(UPostureComponent* Comp)
{
	if (!(bToggle && Comp->IsSprinting()))
	{
		bPressed = bToggle ? !bPressed : true;
	}
}

UPostureComponent::UPostureComponent()
{
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UPostureComponent::BeginPlay()
{
	Super::BeginPlay();

	const_cast<ACharacter*&>(Owner) = CastChecked<ACharacter>(GetOwner());
	PostureState = FCharacterPosture::GetByEnum(Posture);
}

void UPostureComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckState();
}

void UPostureComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPostureComponent, bSprinting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UPostureComponent, Posture, COND_SkipOwner);
}

void UPostureComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveForward", this, &UPostureComponent::ModifyInputScale);
	PlayerInputComponent->BindAxis("MoveRight", this, &UPostureComponent::ModifyInputScale);
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &UPostureComponent::WalkPressed);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &UPostureComponent::WalkReleased);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &UPostureComponent::SprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &UPostureComponent::SprintReleased);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &UPostureComponent::CrouchPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &UPostureComponent::CrouchReleased);
	PlayerInputComponent->BindAction("Prone", IE_Pressed, this, &UPostureComponent::PronePressed);
	PlayerInputComponent->BindAction("Prone", IE_Released, this, &UPostureComponent::ProneReleased);
}

void UPostureComponent::CheckState()
{
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

void UPostureComponent::TrySetSprintingAndTransit(const bool b)
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

void UPostureComponent::ModifyInputScale(float)
{
	auto InputScale = 1.f;
	if (Walk.bPressed && !bSprinting) InputScale *= Walk.SpeedRatio;
	if (Prone.bSwitching) InputScale *= Prone.SpeedRatioWhileSwitching;
	if (InputScale != 1.f)
	{
		auto ControlInputVector = Owner->ConsumeMovementInputVector();
		ControlInputVector.Normalize();
		Owner->AddMovementInput(ControlInputVector, InputScale, true);
	}
}

void UPostureComponent::OnRep_Posture()
{
	if (PostureState->GetEnum() != Posture)
	{
		const auto Old = PostureState;
		const auto New = FCharacterPosture::GetByEnum(Posture);
		Old->Exit(this, New);
		PostureState = New;
		New->Enter(this, Old);
	}
}

void UPostureComponent::ServerSetPosture_Implementation(const EPosture NewPosture)
{
	if (Posture != NewPosture)
	{
		const auto New = FCharacterPosture::GetByEnum(NewPosture);
		PostureState->Exit(this, New);
		New->Enter(this, PostureState);
		PostureState = New;
		Posture = NewPosture;
	}
}

bool UPostureComponent::ServerSetPosture_Validate(const EPosture NewPosture)
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

bool UPostureComponent::CanSprint() const
{
	return Sprint.bPressed && !Prone.bSwitching && Owner->GetInputAxisValue("MoveForward") > 0.f;
}

void UPostureComponent::PlayAnimMontage(UAnimMontage* Anim)
{
	Owner->PlayAnimMontage(Anim);
	LastAnim = Anim;
}

void UPostureComponent::SetProneSwitchDelegate()
{
	FOnMontageBlendingOutStarted Delegate;
	Delegate.BindLambda([this](UAnimMontage*, bool) { Prone.bSwitching = false; });
	Owner->GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(Delegate, LastAnim);
}

void UPostureComponent::ServerSetSprinting_Implementation(const bool b)
{
	SetSprinting_Internal(b);
}

bool UPostureComponent::ServerSetSprinting_Validate(bool)
{
	return true;
}

void UPostureComponent::SetSprinting_Internal(const bool b)
{
	if (bSprinting != b)
	{
		if (b) Owner->GetCharacterMovement()->MaxWalkSpeed *= Sprint.SpeedRatio;
		else Owner->GetCharacterMovement()->MaxWalkSpeed /= Sprint.SpeedRatio;
	}
	bSprinting = b;
}

void UPostureComponent::SetCapsuleHalfHeight(const float Height, const float MeshOffset) const
{
	const auto MeshAdjust = GetDefault<ACharacter>(Owner->GetClass())->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - Height + MeshOffset;
	Owner->OnStartCrouch(MeshAdjust, MeshAdjust * Owner->GetActorScale().Z);
	Owner->GetMesh()->UpdateComponentToWorld();

	if (Owner->Role == ROLE_SimulatedProxy)
	{
		if (auto ClientData = Owner->GetCharacterMovement()->GetPredictionData_Client_Character())
		{
			ClientData->MeshTranslationOffset = FVector::ZeroVector;
			ClientData->OriginalMeshTranslationOffset = FVector::ZeroVector;
		}
	}
	else
	{
		Owner->AddActorLocalOffset({ 0.f, 0.f, (Height - Owner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()) * Owner->GetActorScale().Z });
	}

	Owner->GetCapsuleComponent()->SetCapsuleHalfHeight(Height);
}

bool UPostureComponent::IsOverlapped(const float Height) const
{
	auto Pos = Owner->GetActorLocation();
	Pos.Z += Height - Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const auto Capsule = FCollisionShape::MakeCapsule(Owner->GetCapsuleComponent()->GetScaledCapsuleRadius(), Height);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	return GetWorld()->OverlapBlockingTestByProfile(Pos, FQuat::Identity, "Pawn", Capsule, Params);
}

void UPostureComponent::Transit()
{
	const auto NewState = PostureState->Transit(this);
	if (NewState)
	{
		PostureState->Exit(this, NewState);
		NewState->Enter(this, PostureState);
		Posture = NewState->GetEnum();
		PostureState = NewState;
		if (Owner->Role == ROLE_AutonomousProxy)
		{
			ServerSetPosture(Posture);
		}
	}
}

void UPostureComponent::WalkPressed()
{
	Walk.Press(this);
}

void UPostureComponent::WalkReleased()
{
	Walk.Release(this);
}

void UPostureComponent::SprintPressed()
{
	Sprint.Press(this);
}

void UPostureComponent::SprintReleased()
{
	Sprint.Release(this);
}

void UPostureComponent::CrouchPressed()
{
	Crouch.Press(this);
}

void UPostureComponent::CrouchReleased()
{
	Crouch.Release(this);
}

void UPostureComponent::PronePressed()
{
	Prone.Press(this);
}

void UPostureComponent::ProneReleased()
{
	Prone.Release(this);
}
