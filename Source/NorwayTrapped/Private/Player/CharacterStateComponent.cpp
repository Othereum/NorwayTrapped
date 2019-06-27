// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "CharacterStateComponent.h"

#include "Components/InputComponent.h"
#include "UnrealNetwork.h"

UCharacterStateComponent::UCharacterStateComponent()
{
	bReplicates = true;
}

void UCharacterStateComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	auto BindToggle = [](FName ActionName, auto&& Func)
	{
		auto BindAction = [&](EInputEvent KeyEvent, auto&& ... Args)
		{
			FInputActionBinding InputActionBinding{ ActionName, KeyEvent };
			InputActionBinding.ActionDelegate.GetDelegateForManualSet().BindLambda(Forward<decltype(Func)>(Func), Forward<decltype(Args)>(Args)...);
			PlayerInputComponent->AddActionBinding(InputActionBinding);
			PlayerInputComponent->BindAction(ActionName, KeyEvent, this, &UCharacterStateComponent::UpdateState);
		};
		BindAction(IE_Pressed, true);
		BindAction(IE_Released, false);
	};

	BindToggle("Walk", [this](bool b) {bWantsToWalk = b; });
	BindToggle("Sprint", [this](bool b) {bWantsToSprint = b; });
}

void UCharacterStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UCharacterStateComponent::UpdateState()
{
	switch (Speed)
	{
	case ESpeed::Run:
		if (bWantsToSprint)
		{
			Speed = ESpeed::Sprint;
		}
		else if (bWantsToWalk)
		{
			Speed = ESpeed::Walk;
		}
		break;
	case ESpeed::Walk:
		if (bWantsToSprint)
		{
			Speed = ESpeed::Sprint;
		}
		else if (!bWantsToWalk)
		{
			Speed = ESpeed::Run;
		}
		break;
	case ESpeed::Sprint:
		if (!bWantsToSprint)
		{
			Speed = bWantsToWalk ? ESpeed::Walk : ESpeed::Run;
		}
		break;
	default:
		break;
	}


}
