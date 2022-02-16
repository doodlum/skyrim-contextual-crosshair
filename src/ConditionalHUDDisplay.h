#pragma once

class ConditionalHUDDisplay
{
public:
	static void InstallHooks();

private:
	ConditionalHUDDisplay() = default;

	static void UpdateCrosshair(RE::PlayerCharacter* sky);
	static inline REL::Relocation<decltype(UpdateCrosshair)> _UpdateCrosshair;
};
