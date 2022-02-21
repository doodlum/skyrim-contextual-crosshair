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

void HUDManager::UpdateCrosshair()
{
	auto crosshairInstance = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairInstance");
	if (crosshairInstance != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairInstance.GetDisplayInfo(std::addressof(displayInfo));
		displayInfo.SetAlpha(visible ? alpha : 0.0);
		crosshairInstance.SetDisplayInfo(displayInfo);
	}

	auto crosshairAlert = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairAlert");
	if (crosshairAlert != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairAlert.GetDisplayInfo(std::addressof(displayInfo));
		displayInfo.SetAlpha(visible ? alpha : 0.0);
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

	if (equipped && equipped->GetFormType() == RE::FormType::Weapon) {
		auto weapon = equipped->As<RE::TESObjectWEAP>();
		if (weapon->IsBow()) {
			return attackState == RE::ATTACK_STATE_ENUM::kBowAttached || attackState == RE::ATTACK_STATE_ENUM::kBowDraw || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn;
		} else if (weapon->IsCrossbow()) {
			auto recovery = attackState == RE::ATTACK_STATE_ENUM::kBowAttached || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowReleased || attackState == RE::ATTACK_STATE_ENUM::kNextAttack;
			fadeMult *= recovery ? 0.03125 : 8;
			return (recovery || attackState == RE::ATTACK_STATE_ENUM::kBowDraw || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn);
		}
	}
	return false;
}

bool HUDManager::ValidPickType()
{
	auto CrosshairPickData = RE::CrosshairPickData::GetSingleton();
	auto refr = CrosshairPickData ? CrosshairPickData->target.get() : nullptr;

	return refr && refr->GetFormType() != RE::FormType::ActorCharacter;
}

void HUDManager::UpdateHUD(RE::PlayerCharacter* player, double detectionLevel, RE::GFxValue sneakAnim)
{
	if (SmoothCamInstalled) {
		auto camera = RE::PlayerCamera::GetSingleton();
		camera->lock.Lock();
		visible = camera->currentState != camera->cameraStates[RE::CameraState::kThirdPerson];
		camera->lock.Unlock();
	}

	fadeMult = 1.0;

	bool lookingAtValidRef = ValidPickType();

	auto fadeIn = lookingAtValidRef || ValidCastType(player->magicCasters[0]) || ValidCastType(player->magicCasters[1]) || ValidAttackType(player);
	auto fadeDelta = prevDelta / fadeSpeed;
	
	fadeMult = std::lerp(prevFadeMult, fadeIn && !lookingAtValidRef ? fadeMult : fadeMult * 8, fadeDelta / fadeMult);
	prevFadeMult = fadeMult;

	if (fadeIn) {
		alpha = std::lerp(alpha, maxOpacity, fadeDelta * fadeMult);
	} else {
		alpha = std::lerp(alpha, 0, fadeDelta * fadeMult);
	}
	alpha = std::clamp(alpha, 0.0, visible && !lookingAtValidRef ? maxOpacity : maxOpacity / 2);


	if (player->IsSneaking()) {
		sneakAlpha = std::lerp(sneakAlpha, std::clamp(detectionLevel + alpha, 0.0, maxOpacity), fadeDelta * fadeMult);
	} else {
		sneakAlpha = std::lerp(sneakAlpha, 0, fadeDelta * 16);
	}
	sneakAlpha = std::clamp(sneakAlpha, 0.0, visible ? maxOpacity : maxOpacity / 2);

	UpdateCrosshair();
	UpdateStealthAnim(sneakAnim);
}
