#include "HUDManager.h"

bool HUDManager::TDMCompat()
{
	return g_TDM && g_TDM->GetTargetLockState();
}

bool HUDManager::SmoothCamCompat()
{
	if (g_SmoothCam && g_SmoothCam->IsCameraEnabled())
		if (auto PlayerCamera = RE::PlayerCamera::GetSingleton(); PlayerCamera)
			return PlayerCamera->currentState == PlayerCamera->cameraStates[RE::CameraState::kThirdPerson];
	return false;
}

bool HUDManager::DetectionMeterCompat()
{
	return g_DetectionMeter;
}

bool HUDManager::BTPSCompat()
{
	return g_BTPS && g_BTPS->GetWidget3DEnabled();
}

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

void HUDManager::UpdateCrosshair(double modAlpha)
{
	auto crosshairInstance = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairInstance");
	if (crosshairInstance != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairInstance.GetDisplayInfo(std::addressof(displayInfo));
		displayInfo.SetAlpha(modAlpha);
		crosshairInstance.SetDisplayInfo(displayInfo);
	}

	auto crosshairAlert = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairAlert");
	if (crosshairAlert != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshairAlert.GetDisplayInfo(std::addressof(displayInfo));
		displayInfo.SetAlpha(modAlpha);
		crosshairAlert.SetDisplayInfo(displayInfo);
	}
}

void HUDManager::UpdateStealthAnim(RE::GFxValue sneakAnim, float modAlpha)
{
	RE::GFxValue::DisplayInfo displayInfo;
	sneakAnim.GetDisplayInfo(std::addressof(displayInfo));
	displayInfo.SetAlpha(modAlpha);
	sneakAnim.SetDisplayInfo(displayInfo);
}

bool HUDManager::ValidCastType(RE::ActorMagicCaster* magicCaster)
{
	if (magicCaster)
		return ValidSpellType(magicCaster->currentSpell);
	return false;
}

bool HUDManager::ValidAttackType(RE::PlayerCharacter* player)
{
	auto equipped = player->GetEquippedObject(true);
	auto attackState = player->AsActorState()->actorState1.meleeAttackState;

	if (equipped && equipped->GetFormType() == RE::FormType::Weapon) {
		auto weapon = equipped->As<RE::TESObjectWEAP>();
		if (weapon->IsBow()) {
			return attackState == RE::ATTACK_STATE_ENUM::kBowAttached || attackState == RE::ATTACK_STATE_ENUM::kBowDraw || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowReleased || attackState == RE::ATTACK_STATE_ENUM::kNextAttack || attackState == RE::ATTACK_STATE_ENUM::kBowNextAttack;
		} else if (weapon->IsCrossbow()) {
			return attackState == RE::ATTACK_STATE_ENUM::kBowDraw || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowReleased || attackState == RE::ATTACK_STATE_ENUM::kNextAttack;
		}
	}
	return false;
}

bool HUDManager::ValidPickType()
{
	if (auto CrosshairPickData = RE::CrosshairPickData::GetSingleton(); CrosshairPickData)
		if (auto refr = CrosshairPickData->target.get(); refr)
			return refr->GetFormType() != RE::FormType::ActorCharacter || refr.get()->As<RE::Actor>()->IsDead();
	return false;
}

bool HUDManager::ValidDrawnState(RE::PlayerCharacter* player)
{
	switch (Settings::GetSingleton()->GetCrosshairMode()) {
	case 1:
		if (player->AsActorState()->GetWeaponState() == RE::WEAPON_STATE::kDrawn)
		{
			bool validWeaponType = false;

			if (auto equipped = player->GetEquippedObject(true)) {
				if (auto weapon = equipped->As<RE::TESObjectWEAP>()) {
					if (weapon->IsBow() || weapon->IsCrossbow())
						validWeaponType = true;
					else if (weapon->IsStaff() && weapon->formEnchanting && ValidSpellType(weapon->formEnchanting))
						validWeaponType = true;
				}
			}

			if (auto equipped = player->GetEquippedObject(false)) {
				if (auto weapon = equipped->As<RE::TESObjectWEAP>()) {
					if (weapon->IsStaff() && weapon->formEnchanting && ValidSpellType(weapon->formEnchanting))
						validWeaponType = true;
				}
			}
			if (ValidSpellType(player->GetActorRuntimeData().selectedSpells[0]) || ValidSpellType(player->GetActorRuntimeData().selectedSpells[1]))
				validWeaponType = true;

			return validWeaponType;
		}

		return false;

	case 2:
		return player->AsActorState()->GetWeaponState() == RE::WEAPON_STATE::kDrawn;
	}

	return false;
}

bool SpellContainsArchtype(RE::MagicItem* magicItem, RE::EffectSetting::Archetype archetype)
{
	for (auto effect : magicItem->effects)
		if (effect->baseEffect && effect->baseEffect->HasArchetype(archetype))
			return true;
	return false;
}

bool HUDManager::ValidSpellType(RE::MagicItem* magicItem)
{
	if (magicItem) {
		switch (Settings::GetSingleton()->GetSpellsMode()) {
		case 0:
			return ((magicItem->GetDelivery() == RE::MagicSystem::Delivery::kAimed) && (magicItem->GetCastingType() != RE::MagicSystem::CastingType::kConcentration)) || SpellContainsArchtype(magicItem, RE::EffectSetting::Archetype::kTelekinesis);

		case 1:
			return true;
		}
	}
	return false;
}

long double timer = 0;
#define M_PI 3.14159265358979323846

void HUDManager::UpdateHUD(RE::PlayerCharacter* player, double detectionLevel, RE::GFxValue sneakAnim)
{
	auto settings = Settings::GetSingleton();
	auto SmoothCam = SmoothCamCompat();
	auto TDM = TDMCompat();
	auto DetectionMeter = DetectionMeterCompat();
	auto BTPS = BTPSCompat();

	fadeMult = settings->GetFadeMultiplier();

	auto fadeIn = !(SmoothCam || TDM) && (ValidDrawnState(player) || ValidCastType(player->GetActorRuntimeData().magicCasters[0]) || ValidCastType(player->GetActorRuntimeData().magicCasters[1]) || ValidAttackType(player));

	fadeMult = std::lerp(prevFadeMult, fadeIn ? fadeMult: fadeMult * 16, prevDelta / fadeMult);
	prevFadeMult = fadeMult;

	timer += prevDelta;

	alpha = std::lerp(alpha, (fadeIn || (ValidPickType() && !BTPS)) ? (!TDM && (fadeIn || SmoothCam) ? 100.0 : 50.0) : 0.0, prevDelta * fadeMult);

	auto modAlpha = (alpha * 0.01 * (settings->GetCrosshairMaxOpacity() - settings->GetCrosshairMinOpacity())) + settings->GetCrosshairMinOpacity();

	if (player->IsSneaking()) {
		if (settings->GetSneakMeterMode())
			detectionLevel = 100.0;
		double newAlpha = !((SmoothCam && settings->GetSmoothCamMode() == 0) || TDM) ? std::clamp(DetectionMeter ? modAlpha : detectionLevel + modAlpha, 0.0, 100.0) : DetectionMeter ? 0.0 : detectionLevel / 2;
		newAlpha = (newAlpha * 0.01 * (settings->GetSneakMeterMaxOpacity() - settings->GetSneakMeterMinOpacity())) +  settings->GetSneakMeterMinOpacity();
		sneakAlpha = std::lerp(sneakAlpha, newAlpha, prevDelta * fadeMult);
		auto detectionFreq = (detectionLevel / 200) + 0.5f;
		auto pulse = (settings->GetSneakMeterRange() * sin(2 * (M_PI * 2) * detectionFreq * settings->GetSneakMeterFrequency() * 0.25f * timer)) + (1.0f - settings->GetSneakMeterRange());
		sneakAlpha *= min((double)pulse, 1.0f);
		sneakAlpha = max(alpha * 0.01 * settings->GetSneakMeterMaxOpacity(), sneakAlpha);
	} else {
		sneakAlpha = std::lerp(sneakAlpha, 0.0, prevDelta * 16);
	}

	UpdateCrosshair(modAlpha);
	UpdateStealthAnim(sneakAnim, (float)sneakAlpha);
}
