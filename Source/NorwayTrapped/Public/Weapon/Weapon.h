// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* Mesh;
	
public:	
	AWeapon();

	/*
	 * Called when player has just switched to this weapon.
	 * @return: Return true to allow switching away from this weapon
	 */
	virtual bool Deploy() { return true; }

	/*
	 * Called when weapon tries to holster.
	 * @param Weapon: The weapon we are trying switch to.
	 * @return: Return true to allow weapon to holster
	 */
	virtual bool Holster(AWeapon* Weapon) { return true; }

	uint8 GetSlot() const { return Slot; }

private:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 Slot;
};
