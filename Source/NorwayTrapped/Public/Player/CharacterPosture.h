// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EPosture : uint8;
class UChrStateComp;

class FCharacterPosture
{
public:
	static FCharacterPosture* GetByEnum(EPosture Enum);

	virtual ~FCharacterPosture() = default;
	virtual EPosture GetEnum() const = 0;
	virtual FCharacterPosture* Transit(UChrStateComp* Comp) const = 0;
	virtual void Enter(UChrStateComp* Comp) const = 0;
	virtual bool CanEnter(UChrStateComp* Comp) const { return true; }
	virtual void Exit(UChrStateComp* Comp) const {}
};

class FStand final : public FCharacterPosture
{
public:
	static FCharacterPosture* GetObject();

private:
	friend FCharacterPosture* FCharacterPosture::GetByEnum(EPosture);
	EPosture GetEnum() const override;
	FCharacterPosture* Transit(UChrStateComp* Comp) const override;
	void Enter(UChrStateComp* Comp) const override;
	bool CanEnter(UChrStateComp* Comp) const override;
};

class FCrouch final : public FCharacterPosture
{
public:
	static FCharacterPosture* GetObject();

private:
	friend FCharacterPosture* FCharacterPosture::GetByEnum(EPosture);
	EPosture GetEnum() const override;
	FCharacterPosture* Transit(UChrStateComp* Comp) const override;
	void Enter(UChrStateComp* Comp) const override;
	bool CanEnter(UChrStateComp* Comp) const override;
	void Exit(UChrStateComp* Comp) const override;
};

class FProne final : public FCharacterPosture
{
public:
	static FCharacterPosture* GetObject();

private:
	friend FCharacterPosture* FCharacterPosture::GetByEnum(EPosture);
	EPosture GetEnum() const override;
	FCharacterPosture* Transit(UChrStateComp* Comp) const override;
	void Enter(UChrStateComp* Comp) const override;
	bool CanEnter(UChrStateComp* Comp) const override;
	void Exit(UChrStateComp* Comp) const override;
};
