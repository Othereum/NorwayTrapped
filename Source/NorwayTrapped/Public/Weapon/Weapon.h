// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	NeverDeployed, Deploying, Idle, Firing, Reloading, Holstering, Unequipped
};

class UAnimMontage;

UCLASS()
class AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* Mesh;
	
public:	
	AWeapon();

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void PostInitializeComponents() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeploy();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHolster(AWeapon* To);

	virtual void FireP() {}
	virtual void FireR() {}
	virtual void AimP() {}
	virtual void AimR() {}
	virtual void Reload() {}

	virtual bool CanDeploy() const;
	virtual bool CanHolster() const;

	uint8 GetSlot() const { return Slot; }
	EWeaponState GetState() const { return State; }
	USkeletalMeshComponent* GetMesh() const { return Mesh; }
	bool IsVisible() const;
	void SetVisibility(bool bNewVisibility) const;
	class AFpsCharacter* GetCharacter() const;

protected:
	virtual void Deploy();
	virtual void Holster(AWeapon* To);
	void PlayOwnerAnim(UAnimMontage* Anim, float Time, bool bConsiderBlendOutTime = true) const;
	void PlayWepAnim(UAnimMontage* Anim) const;
	void StopWepAnim(float BlendOutTime, UAnimMontage* Anim = nullptr) const;

	UPROPERTY(VisibleInstanceOnly, Replicated, Transient, BlueprintReadOnly)
	EWeaponState State;

private:
	void Init();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditAnywhere)
	float DeployTime;

	UPROPERTY(EditAnywhere)
	float HolsterTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 Slot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UAnimMontage* DeployAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UAnimMontage* HolsterAnim;

	FTimerHandle StateSetTimer;

	AFpsCharacter* Owner;
};
