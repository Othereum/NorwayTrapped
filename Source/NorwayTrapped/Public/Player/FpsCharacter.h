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
	float GetHp() const { return Hp; }
	bool IsAlive() const { return bAlive; }

	virtual void Kill();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastKill();

	UFUNCTION(BlueprintImplementableEvent)
	void OnKill();

protected:
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
};
