// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "FpsAnimInstance.h"
#include "FpsCharacter.h"
#include "Weapon.h"
#include "WeaponComponent.h"

void UFpsAnimInstance::NativeUpdateAnimation(float)
{
	const auto Owner = Cast<AFpsCharacter>(TryGetPawnOwner());
	if (!Owner) return;

	const auto Velocity = Owner->GetVelocity();
	const auto Rotation = Owner->GetActorRotation();
	Speed = Velocity.Size();
	Direction = CalculateDirection(Velocity, Rotation);
	Aim = (Owner->GetBaseAimRotation() - Rotation).GetNormalized();

	if (const auto Wep = Owner->GetWeaponComponent()->GetActiveWeapon())
	{
		const auto LeftHand = Wep->GetMesh()->GetSocketTransform(WeaponLeftHandIKSocketName);
		FVector IKLocation;
		FRotator IKRotation;
		Owner->GetMesh()->TransformToBoneSpace(RightHandBoneName, LeftHand.GetLocation(), LeftHand.GetRotation().Rotator(),
		                                       IKLocation, IKRotation);
		LeftHandIK.SetLocation(IKLocation);
		LeftHandIK.SetRotation(IKRotation.Quaternion());

		switch (Wep->GetState())
		{
		case EWeaponState::Deploying:
		case EWeaponState::Holstering:
		case EWeaponState::Reloading:
			bFABRIK = false;
			break;
		default:
			bFABRIK = true;
		}
	}

	Posture = Owner->GetPostureComponent()->GetPostureEnum();
	bSprinting = Owner->GetPostureComponent()->IsSprinting();
	bSwitchingProne = Owner->GetPostureComponent()->Prone.bSwitching;
}
