// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponComponent final : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

private:
	void InitializeComponent() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetupPlayerInputComponent(class UInputComponent* Input);

	/*
	 * [Client] Sets the active weapon by slot number.
	 * @param Slot: Slot number of the weapon to equip
	 */
	void SetActiveWeapon(uint8 Slot);

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
	
	class ACharacter* const Owner = nullptr;

private:
	UFUNCTION()
	void OnRep_Active(uint8 Old);

	UPROPERTY(EditDefaultsOnly)
	uint8 WeaponSlots;

	UPROPERTY(VisibleInstanceOnly, Replicated, Transient)
	TArray<AWeapon*> Weapons;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_Active, Transient)
	uint8 Active;
};
