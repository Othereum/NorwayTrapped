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

public: // Public interfaces
	ANorwayTrappedCharacter();

public: // Public variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSprintSpeed = 500.f;

private: // Virtual function override
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

private: // Input
	void MoveForward(float Value);
	void MoveRight(float Value);

	void Walk();
	bool CanWalk() const;

	void WalkPressed();
	void WalkReleased();
	uint8 bWantsToWalk : 1;

	bool CanSprint() const;
	uint8 bSprinting : 1;

	void SprintPressed();
	void SprintReleased();
	uint8 bWantsToSprint : 1;

public: // Simple getter/setters
	class UCameraComponent* GetCamera() const { return Camera; }

};
