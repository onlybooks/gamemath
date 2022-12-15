
#include "Precompiled.h"

float InputManager::GetAxis(InputAxis InInputAxis) const
{
	assert(InInputAxis < InputAxis::LastAxis);
	return AxisMap[static_cast<size_t>(InInputAxis)]();
}

bool InputManager::IsPressed(InputButton InInputButton) const
{
	assert(InInputButton < InputButton::LastButton);
	size_t targetIndex = static_cast<size_t>(InInputButton);
	return (!PrevButtonStatus[targetIndex] && PressedButtonMap[targetIndex]());
}

bool InputManager::IsPressing(InputButton InInputButton) const
{
	assert(InInputButton < InputButton::LastButton);
	size_t targetIndex = static_cast<size_t>(InInputButton);
	return (PrevButtonStatus[targetIndex] && PressedButtonMap[targetIndex]());
}

bool InputManager::IsReleased(InputButton InInputButton) const
{
	assert(InInputButton < InputButton::LastButton);
	size_t targetIndex = static_cast<size_t>(InInputButton);
	return (PrevButtonStatus[targetIndex] && !PressedButtonMap[targetIndex]());
}

bool InputManager::IsInputReady() const
{
	if (!AxisMap[0])
	{
		return false;
	}

	if (!PressedButtonMap[0])
	{
		return false;
	}

	return true;
}

void InputManager::SetInputAxis(InputAxis InInputAxis, std::function<float()> InAxisFn)
{
	assert(InInputAxis < InputAxis::LastAxis);
	AxisMap[static_cast<size_t>(InInputAxis)] = InAxisFn;
}

void InputManager::SetInputButton(InputButton InInputButton, std::function<bool()> InPressedFn)
{
	assert(InInputButton < InputButton::LastButton);
	PressedButtonMap[static_cast<size_t>(InInputButton)] = InPressedFn;
}

void InputManager::UpdateInput()
{
	size_t lastIndex = static_cast<size_t>(InputButton::LastButton);
	for (size_t bi = 0; bi < lastIndex; ++bi)
	{
		PrevButtonStatus[bi] = PressedButtonMap[bi]();
	}
}
