// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once 

#include "GameFramework/HUD.h"
#include "NorwayTrappedHUD.generated.h"

UCLASS()
class ANorwayTrappedHUD : public AHUD
{
	GENERATED_BODY()

public:
	ANorwayTrappedHUD();

protected:
	void DrawHUD() override;

private:
	class UTexture2D* CrosshairTex;
};

