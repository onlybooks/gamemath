
#include "Precompiled.h"

bool SystemInputManager::IsPressed(SystemInputButton InSystemInputButton) const
{
	assert(InSystemInputButton < SystemInputButton::LastButton);
	size_t targetIndex = static_cast<size_t>(InSystemInputButton);
	return (!PrevButtonStatus[targetIndex] && PressedButtonMap[targetIndex]());
}

bool SystemInputManager::IsPressing(SystemInputButton InSystemInputButton) const
{
	assert(InSystemInputButton < SystemInputButton::LastButton);
	size_t targetIndex = static_cast<size_t>(InSystemInputButton);
	return (PrevButtonStatus[targetIndex] && PressedButtonMap[targetIndex]());
}

bool SystemInputManager::IsReleased(SystemInputButton InSystemInputButton) const
{
	assert(InSystemInputButton < SystemInputButton::LastButton);
	size_t targetIndex = static_cast<size_t>(InSystemInputButton);
	return (PrevButtonStatus[targetIndex] && !PressedButtonMap[targetIndex]());
}

void SystemInputManager::SetSystemInputButton(SystemInputButton SystemInputButton, std::function<bool()> InPressedFn)
{
	assert(SystemInputButton < SystemInputButton::LastButton);
	PressedButtonMap[static_cast<size_t>(SystemInputButton)] = InPressedFn;
}

void SystemInputManager::UpdateSystemInput()
{
	size_t lastIndex = static_cast<size_t>(SystemInputButton::LastButton);
	for (size_t bi = 0; bi < lastIndex; ++bi)
	{
		PrevButtonStatus[bi] = PressedButtonMap[bi]();
	}
}

bool SystemInputManager::IsInputReady() const
{
	if (!PressedButtonMap[0])
	{
		return false;
	}

	return true;
}