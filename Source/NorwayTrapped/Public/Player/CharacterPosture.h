// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EPosture : uint8;
enum class EDirection : uint8;
class UPostureComponent;

class FCharacterPosture
{
public:
	static FCharacterPosture* GetByEnum(EPosture Enum);

	virtual ~FCharacterPosture() = default;
	virtual EPosture GetEnum() const = 0;
	virtual FCharacterPosture* Transit(UPostureComponent* Comp) const { return nullptr; }
	virtual void Enter(UPostureComponent* Comp, FCharacterPosture* Before) const {}
	virtual bool CanEnter(UPostureComponent* Comp) const { return true; }
	virtual void Exit(UPostureComponent* Comp, FCharacterPosture* After) const {}
};

class FStand final : public FCharacterPosture
{
public:
	static FCharacterPosture* GetObject();

private:
	friend FCharacterPosture* FCharacterPosture::GetByEnum(EPosture);
	EPosture GetEnum() const override;
	FCharacterPosture* Transit(UPostureComponent* Comp) const override;
	void Enter(UPostureComponent* Comp, FCharacterPosture* Before) const override;
	bool CanEnter(UPostureComponent* Comp) const override;
	void Exit(UPostureComponent* Comp, FCharacterPosture* After) const override;
};

class FCrouch final : public FCharacterPosture
{
public:
	static FCharacterPosture* GetObject();

private:
	friend FCharacterPosture* FCharacterPosture::GetByEnum(EPosture);
	EPosture GetEnum() const override;
	FCharacterPosture* Transit(UPostureComponent* Comp) const override;
	void Enter(UPostureComponent* Comp, FCharacterPosture* Before) const override;
	bool CanEnter(UPostureComponent* Comp) const override;
	void Exit(UPostureComponent* Comp, FCharacterPosture* After) const override;
};

class FProne final : public FCharacterPosture
{
public:
	static FCharacterPosture* GetObject();

private:
	friend FCharacterPosture* FCharacterPosture::GetByEnum(EPosture);
	EPosture GetEnum() const override;
	FCharacterPosture* Transit(UPostureComponent* Comp) const override;
	void Enter(UPostureComponent* Comp, FCharacterPosture* Before) const override;
	bool CanEnter(UPostureComponent* Comp) const override;
	void Exit(UPostureComponent* Comp, FCharacterPosture* After) const override;
};
