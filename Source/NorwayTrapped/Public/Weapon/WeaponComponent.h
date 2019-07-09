// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponComponent final : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

private:
	void InitializeComponent() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetupPlayerInputComponent(class UInputComponent* Input);

	/*
	 * [Server] Gives a weapon. Replaces if already in the same slot.
	 * @param WeaponClass: Class of weapon to give
	 * @return: The weapon given
	 */
	UFUNCTION(BlueprintCallable)
	AWeapon* Give(TSubclassOf<AWeapon> WeaponClass);

	/*
	 * [Shared] Returns active weapon.
	 * @return: The weapon currently equipped
	 */
	UFUNCTION(BlueprintCallable)
	AWeapon* GetActiveWeapon() const { return Active < Weapons.Num() ? Weapons[Active] : nullptr; }

	void SelectWeapon(uint8 Slot);

	class AFpsCharacter* const Owner = nullptr;

private:
	void AimP(); void AimR();
	void FireP(); void FireR();
	void Reload();

	UFUNCTION(Server, Reliable, WithValidation) void ServerFireP();
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireR();
	UFUNCTION(Server, Reliable, WithValidation) void ServerAimP();
	UFUNCTION(Server, Reliable, WithValidation) void ServerAimR();
	UFUNCTION(Server, Reliable, WithValidation) void ServerReload();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetActiveWeapon(uint8 Slot);

	UFUNCTION()
	void OnRep_Weapons();

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing=OnRep_Weapons, Transient)
	TArray<AWeapon*> Weapons;

	UPROPERTY(EditDefaultsOnly, meta = (UIMax = 9, ClampMax = 9))
	uint8 WeaponSlots;

	UPROPERTY(VisibleInstanceOnly, Replicated, Transient)
	uint8 Active;
};
