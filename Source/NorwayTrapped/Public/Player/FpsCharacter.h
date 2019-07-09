// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FpsCharacter.generated.h"

UCLASS()
class AFpsCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UPostureComponent* Posture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UWeaponComponent* Weapon;

public:
	AFpsCharacter();

	UCameraComponent* GetCamera() const { return Camera; }
	UPostureComponent* GetPosture() const { return Posture; }
	UWeaponComponent* GetWeapon() const { return Weapon; }

private:
	void SetupPlayerInputComponent(class UInputComponent* Input) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
};
