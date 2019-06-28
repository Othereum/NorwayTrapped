// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "NorwayTrappedCharacter.generated.h"

UCLASS()
class ANorwayTrappedCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;

public:
	ANorwayTrappedCharacter();

	class UCameraComponent* GetCamera() const { return Camera; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSprintSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float BaseLookUpRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bToggleToWalk : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bToggleToSprint : 1;

private:
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bSprinting : 1;
	uint8 bWantsToWalk : 1;
	uint8 bWantsToSprint : 1;

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void WalkPressed();
	void WalkReleased();
	void Walk();
	bool CanWalk() const;

	void SprintPressed();
	void SprintReleased();
	bool CanSprint() const;
	void SetSprinting(bool b);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool b);
};
