// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	virtual void SetupPlayerInputComponent(class UInputComponent* Input);

	/*
	 * [Server] Gives a weapon. Replaces if already in the same slot.
	 * @param WeaponClass: Class of weapon to give
	 * @return: The weapon given
	 */
	UFUNCTION(BlueprintCallable)
	virtual AWeapon* Give(TSubclassOf<AWeapon> WeaponClass);

	/*
	 * [Shared] Returns active weapon.
	 * @return: The weapon currently equipped
	 */
	UFUNCTION(BlueprintCallable)
	AWeapon* GetActiveWeapon() const { return Active < Weapons.Num() ? Weapons[Active] : nullptr; }

	virtual void SelectWeapon(uint8 Slot);
	virtual void FireR();

	class AFpsCharacter* const Owner = nullptr;

protected:
	void InitializeComponent() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void AimP();
	virtual void AimR();
	virtual void FireP();
	virtual void Reload();

	UFUNCTION()
	virtual void OnRep_Weapons();

private:
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireP();
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireR();
	UFUNCTION(Server, Reliable, WithValidation) void ServerAimP();
	UFUNCTION(Server, Reliable, WithValidation) void ServerAimR();
	UFUNCTION(Server, Reliable, WithValidation) void ServerReload();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireP();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireR();
	UFUNCTION(NetMulticast, Reliable) void MulticastAimP();
	UFUNCTION(NetMulticast, Reliable) void MulticastAimR();
	UFUNCTION(NetMulticast, Reliable) void MulticastReload();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetActiveWeapon(uint8 Slot);

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing=OnRep_Weapons, Transient)
	TArray<AWeapon*> Weapons;

	UPROPERTY(EditDefaultsOnly, meta = (UIMax = 9, ClampMax = 9))
	uint8 WeaponSlots;

	UPROPERTY(VisibleInstanceOnly, Replicated, Transient)
	uint8 Active;
};
