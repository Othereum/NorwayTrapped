// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FpsCharacter.generated.h"

UCLASS()
class AFpsCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UPostureComponent* PostureComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCameraComponent* AimCamera;

public:
	AFpsCharacter();

	UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	UPostureComponent* GetPostureComponent() const { return PostureComponent; }
	UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

	FVector GetPawnViewLocation() const override;

	float GetHp() const { return Hp; }
	bool IsAlive() const { return bAlive; }
	virtual void Kill();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastKill();

	UFUNCTION(BlueprintImplementableEvent)
	void OnKill();

	void Aim();
	void UnAim();
	float GetAimBlendAlpha() const { return AimCamBlendAlpha; }

protected:
	static const FName CameraComponentName;
	static const FName PostureComponentName;
	static const FName WeaponComponentName;
	static const FName AimCameraName;

	void Tick(float DeltaSeconds) override;
	void SetupPlayerInputComponent(class UInputComponent* Input) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	bool ShouldTakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);

	UPROPERTY(EditAnywhere)
	TMap<FName, float> HitBoneDmgMul;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Hp = 100;

	UPROPERTY(VisibleInstanceOnly, Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bAlive : 1;

	uint8 bBlendingAimCam : 1;
	uint8 bBlendingAimCamForward : 1;
	float AimCamBlendAlpha;
};
