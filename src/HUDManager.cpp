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

bool HUDManager::ValidCastType(RE::ActorMagicCaster* magicCaster)
{
	if (magicCaster && magicCaster->currentSpell){
		RE::MagicItem* magicItem = magicCaster->currentSpell;
		return (magicItem->GetDelivery() == RE::MagicSystem::Delivery::kAimed) && (magicItem->GetCastingType() != RE::MagicSystem::CastingType::kConcentration);
	}
	return false;
}

bool HUDManager::ValidAttackType(RE::PlayerCharacter* player)
{
	auto equipped = player->GetEquippedObject(true);
	auto attackState = player->actorState1.meleeAttackState;
	auto weaponState = player->actorState2.weaponState;

	if (equipped && equipped->GetFormType() == RE::FormType::Weapon) {
		auto weapon = equipped->As<RE::TESObjectWEAP>();
		if (weapon->IsBow()) {
			return attackState == RE::ATTACK_STATE_ENUM::kBowAttached || attackState == RE::ATTACK_STATE_ENUM::kBowDraw || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn;
		} else if (weapon->IsCrossbow()) {
			auto recovery = attackState == RE::ATTACK_STATE_ENUM::kBowAttached || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowReleased || attackState == RE::ATTACK_STATE_ENUM::kNextAttack || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough;
			fadeMult *= recovery ? 0.25 : 8;
			return weaponState != RE::WEAPON_STATE::kDrawing && (recovery || attackState == RE::ATTACK_STATE_ENUM::kBowDraw || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn);
		}
	}
	return false;
}

void HUDManager::UpdateCrosshair()
{
	auto crosshairInstance = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairInstance");
	if (crosshairInstance != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairInstance.GetDisplayInfo(std::addressof(displayInfo));
		displayInfo.SetAlpha(alpha);
		crosshairInstance.SetDisplayInfo(displayInfo);
	}

	auto crosshairAlert = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairAlert");
	if (crosshairAlert != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairAlert.GetDisplayInfo(std::addressof(displayInfo));
		displayInfo.SetAlpha(alpha);
		crosshairAlert.SetDisplayInfo(displayInfo);
	}
}

void HUDManager::UpdateStealthAnim(RE::GFxValue sneakAnim)
{
	RE::GFxValue::DisplayInfo displayInfo;
	sneakAnim.GetDisplayInfo(std::addressof(displayInfo));
	displayInfo.SetAlpha(sneakAlpha);
	sneakAnim.SetDisplayInfo(displayInfo);
}

void HUDManager::UpdateHUD(RE::PlayerCharacter* player, double detectionLevel, RE::GFxValue sneakAnim)
{
	fadeMult = 1.0;

	auto pickData = RE::CrosshairPickData::GetSingleton()->target;

	auto isVisible = pickData || ValidCastType(player->magicCasters[0]) || ValidCastType(player->magicCasters[1]) || ValidAttackType(player);

	if (isVisible) {
		alpha = std::lerp(alpha, maxOpacity, prevDelta / fadeSpeed * fadeMult);
	} else {
		fadeMult *= 3;
		alpha = std::lerp(alpha, 0, prevDelta / fadeSpeed * fadeMult);
	}
	alpha = std::clamp(alpha, 0.0, maxOpacity);

	if (player->IsSneaking()) {
		sneakAlpha = std::lerp(sneakAlpha, std::clamp(detectionLevel + alpha, 0.0, maxOpacity), prevDelta / fadeSpeed * fadeMult);
	} else {
		sneakAlpha = std::lerp(sneakAlpha, 0, prevDelta / fadeSpeed * 16);
	}
	sneakAlpha = std::clamp(sneakAlpha, 0.0, maxOpacity);

	UpdateCrosshair();
	UpdateStealthAnim(sneakAnim);
}
