#include "HUDManager.h"


[[nodiscard]] RE::GFxValue GetGFxValue(const char* a_pathToVar)
{
	RE::GFxValue object;

	auto ui = RE::UI::GetSingleton();
	auto hud = ui ? ui->GetMenu<RE::HUDMenu>() : nullptr;
	auto view = hud ? hud->uiMovie : nullptr;
	if (view) 
		view->GetVariable(std::addressof(object), a_pathToVar);

	return object;
}

using WEAPON_STATE = RE::WEAPON_STATE;
using ATTACK_STATE = RE::ATTACK_STATE_ENUM;

void HUDManager::UpdateHUD(RE::PlayerCharacter* player, float delta)
{
	auto isVisible = false;
	auto attackState = player->actorState1.meleeAttackState;

	if (attackState == ATTACK_STATE::kBowAttached || attackState == ATTACK_STATE::kBowDrawn || attackState == ATTACK_STATE::kFiring || attackState == ATTACK_STATE::kFire || attackState == ATTACK_STATE::kFired) {
		isVisible = true;
	}

	auto crosshair = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairInstance");

	if (crosshair != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshair.GetDisplayInfo(std::addressof(displayInfo));

		double alpha = displayInfo.GetAlpha();
		if (isVisible) {
			alpha += delta * maxOpacity * fadeSpeed;
		} else {
			alpha -= delta * maxOpacity * fadeSpeed * 2;
		}
		alpha = std::clamp(alpha, 0.0, maxOpacity);

		logger::info(FMT_STRING("{}"sv), alpha);

		displayInfo.SetAlpha(alpha);
		crosshair.SetDisplayInfo(displayInfo);
	}
}
