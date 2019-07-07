// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class AWeapon final : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* Mesh;
	
public:	
	AWeapon();

private:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
};
