#include "ConditionalHUDDisplay.h"

void ConditionalHUDDisplay::InstallHooks()
{
#if defined(SKYRIMAE)
#elif defined(SKYRIMVR)
#else
	REL::Relocation<std::uintptr_t> UpdateCrosshair_hook{ REL::ID(39375), 0xBDA };
#endif

	SKSE::AllocTrampoline(14);
	auto& trampoline = SKSE::GetTrampoline();
	_UpdateCrosshair = trampoline.write_call<5>(UpdateCrosshair_hook.address(), UpdateCrosshair);
}

[[nodiscard]] RE::GFxValue GetMovieView(const char* a_pathToVar)
{
	RE::GFxValue object;

	auto ui = RE::UI::GetSingleton();
	auto hud = ui ? ui->GetMenu<RE::HUDMenu>() : nullptr;
	auto view = hud ? hud->uiMovie : nullptr;
	if (view) 
		view->GetVariable(std::addressof(object), a_pathToVar);

	return object;
}

void ConditionalHUDDisplay::UpdateCrosshair(RE::PlayerCharacter* player)
{
	if (player) {
		auto crosshair = GetMovieView("_root.HUDMovieBaseInstance.CrosshairInstance");

		RE::GFxValue::DisplayInfo displayInfo;
		crosshair.GetDisplayInfo(std::addressof(displayInfo));
		displayInfo.SetAlpha(0);
		crosshair.SetDisplayInfo(displayInfo);
	}	
}
