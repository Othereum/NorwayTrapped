// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "NorwayTrappedCharacter.generated.h"

UCLASS()
class ANorwayTrappedCharacter : public ACharacter
{
	GENERATED_BODY()

private: // Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UCharacterStateComponent* State;

public: // Public interfaces
	ANorwayTrappedCharacter();

public: // Public variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate = 45.f;

private: // Virtual function override
	virtual void BeginPlay();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

private: // Internal functions
	void MoveForward(float Val);
	void MoveRight(float Val);

private: // Internal variables

public: // Simple getter/setters
	class UCameraComponent* GetCamera() const { return Camera; }

};
