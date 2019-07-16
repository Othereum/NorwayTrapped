// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "FpsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"
#include "Gun.h"
#include "PostureComponent.h"
#include "WeaponComponent.h"

//////////////////////////////////////////////////////////////////////////
// AFpsCharacter

const FName AFpsCharacter::CameraComponentName = "CameraComponent";
const FName AFpsCharacter::PostureComponentName = "PostureComponent";
const FName AFpsCharacter::WeaponComponentName = "WeaponComponent";
const FName AFpsCharacter::AimCameraName = "AimCamera";

AFpsCharacter::AFpsCharacter()
	:CameraComponent{ CreateDefaultSubobject<UCameraComponent>(CameraComponentName) },
	PostureComponent{ CreateDefaultSubobject<UPostureComponent>(PostureComponentName) },
	WeaponComponent{ CreateDefaultSubobject<UWeaponComponent>(WeaponComponentName) },
	AimCamera{ CreateDefaultSubobject<UCameraComponent>(AimCameraName) },
	bAlive{ true }
{
	GetMesh()->bReturnMaterialOnMove = true;
	CameraComponent->SetupAttachment(GetMesh(), "Eye");
	WeaponComponent->SetupAttachment(GetMesh(), "RightHand");
	AimCamera->SetupAttachment(WeaponComponent);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFpsCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bBlendingAimCam)
	{
		if (const auto Gun = Cast<AGun>(WeaponComponent->GetActiveWeapon()))
		{
			AimCamBlendAlpha = FMath::Clamp(AimCamBlendAlpha + DeltaSeconds / Gun->GetAimTime()
				* (bBlendingAimCamForward ? 1.f : -1.f), 0.f, 1.f);
			AimCamera->SetWorldLocation(FMath::CubicInterp(CameraComponent->GetComponentLocation(), FVector::ZeroVector, Gun->GetAimLocation(), FVector::ZeroVector, AimCamBlendAlpha));
			AimCamera->SetFieldOfView(CameraComponent->FieldOfView * FMath::CubicInterp(1.f, 0.f, Gun->GetAimFovRatio(), 0.f, AimCamBlendAlpha));
			
			if (AimCamBlendAlpha == 0.f || AimCamBlendAlpha == 1.f)
			{
				if (!bBlendingAimCamForward)
				{
					AimCamera->Deactivate();
					CameraComponent->Activate();
				}
				bBlendingAimCam = false;
			}
		}
	}
}

void AFpsCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	Input->BindAxis("MoveForward", this, &AFpsCharacter::MoveForward);
	Input->BindAxis("MoveRight", this, &AFpsCharacter::MoveRight);

	Input->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	Input->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PostureComponent->SetupPlayerInputComponent(Input);
	WeaponComponent->SetupPlayerInputComponent(Input);
}

void AFpsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFpsCharacter, Hp);
	DOREPLIFETIME(AFpsCharacter, bAlive);
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

//////////////////////////////////////////////////////////////////////////
// HP / Damage

void AFpsCharacter::Kill()
{
	Hp = 0.f;
	bAlive = false;
	OnKill();
}

void AFpsCharacter::Aim()
{
	bBlendingAimCam = true;
	bBlendingAimCamForward = true;

	CameraComponent->Deactivate();
	AimCamera->Activate();
}

void AFpsCharacter::UnAim()
{
	bBlendingAimCam = true;
	bBlendingAimCamForward = false;
}

void AFpsCharacter::MulticastKill_Implementation()
{
	Kill();
}

float AFpsCharacter::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage > 0.f)
	{
		FHitResult Hit;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, DamageCauser, Hit, ImpulseDir);
		if (Hit.BoneName != NAME_None && HitBoneDmgMul.Contains(Hit.BoneName))
			Damage *= HitBoneDmgMul[Hit.BoneName];

		Hp -= Damage;
		if (Hp <= 0.f) MulticastKill();
	}
	return Damage;
}

bool AFpsCharacter::ShouldTakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	return bAlive && Super::ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
