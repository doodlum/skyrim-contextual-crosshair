#include "HUDManager.h"
#include "RE/RE.h"

using namespace std;

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




bool ValidCastType(RE::PlayerCharacter* player, RE::TESForm* form)
{
	if (form && form->Is(RE::FormType::Spell)) {
		auto spell = static_cast<RE::SpellItem*>(form);
		return IsCasting(player, spell) && (spell->GetDelivery() == RE::MagicSystem::Delivery::kAimed) && (spell->GetCastingType() != RE::MagicSystem::CastingType::kConcentration);
	}
	return false;
}

using ATTACK_STATE = RE::ATTACK_STATE_ENUM;

bool ValidAttackType(RE::PlayerCharacter* player)
{

	auto attackState = player->actorState1.meleeAttackState;
	return attackState == ATTACK_STATE::kBowAttached || attackState == ATTACK_STATE::kBowDrawn;
}

void HUDManager::UpdateCrosshair(RE::PlayerCharacter* player)
{
	auto pickData = RE::CrosshairPickData::GetSingleton()->target;

	auto leftHand = player->GetEquippedObject(true);
	auto rightHand = player->GetEquippedObject(false);

	auto isVisible = pickData || ValidAttackType(player) || ValidCastType(player, leftHand) || ValidCastType(player, rightHand);

	if (isVisible) {
		alpha += prevDelta * (maxOpacity / fadeSpeed);
	} else {
		alpha -= prevDelta * (maxOpacity / fadeSpeed) * 3;
	}
	alpha = clamp(alpha, 0.0, maxOpacity);

	auto crosshairInstance = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairInstance");
	if (crosshairInstance != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairInstance.GetDisplayInfo(addressof(displayInfo));
		displayInfo.SetAlpha(alpha);
		crosshairInstance.SetDisplayInfo(displayInfo);
	}

	auto crosshairAlert = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairAlert");
	if (crosshairAlert != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairAlert.GetDisplayInfo(addressof(displayInfo));
		displayInfo.SetAlpha(alpha);
		crosshairAlert.SetDisplayInfo(displayInfo);
	}
}

void HUDManager::UpdateStealthAnim(RE::PlayerCharacter* player, RE::GFxValue sneakAnim, double detectionLevel)
{
	auto isVisible = player->IsSneaking();

	if (isVisible) {
		sneakAlpha = lerp(clamp(detectionLevel + alpha, 0.0, maxOpacity), sneakAlpha, prevDelta / fadeSpeed);
	} else {
		sneakAlpha -= prevDelta * (maxOpacity / fadeSpeed) * 3;
	}
	sneakAlpha = clamp(sneakAlpha, 0.0, maxOpacity);

	RE::GFxValue::DisplayInfo displayInfo;
	sneakAnim.GetDisplayInfo(addressof(displayInfo));
	displayInfo.SetAlpha(sneakAlpha);
	sneakAnim.SetDisplayInfo(displayInfo);
}

void HUDManager::UpdateHUD(RE::PlayerCharacter* player, RE::GFxValue sneakAnim, double detectionLevel) 
{
	UpdateCrosshair(player);
	UpdateStealthAnim(player, sneakAnim, detectionLevel);
}
