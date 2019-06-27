// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CharacterStateComponent.generated.h"

UENUM(BlueprintType)
enum class ESpeed : uint8
{
	Run, Walk, Sprint
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NORWAYTRAPPED_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterStateComponent();
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateState();
	
	ESpeed Speed;
	uint8 bWantsToWalk : 1;
	uint8 bWantsToSprint : 1;
};
