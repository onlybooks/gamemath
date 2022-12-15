#pragma once

namespace CK
{

enum class SystemInputButton : UINT32
{
	F1 = 0,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	LastButton
};

class SystemInputManager
{
public:
	bool IsPressed(SystemInputButton InSystemInputButton) const;
	bool IsPressing(SystemInputButton InSystemInputButton) const;
	bool IsReleased(SystemInputButton InSystemInputButton) const;
	bool IsInputReady() const;
	void SetSystemInputButton(SystemInputButton InSystemInputButton, std::function<bool()> InPressedFn);
	void UpdateSystemInput();

private:
	std::array<std::function<bool()>, static_cast<size_t>(SystemInputButton::LastButton)> PressedButtonMap = { 0 };
	std::array<bool, static_cast<size_t>(SystemInputButton::LastButton)> PrevButtonStatus = { 0 };
};

}