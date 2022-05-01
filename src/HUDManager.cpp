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
			return attackState == RE::ATTACK_STATE_ENUM::kBowAttached || attackState == RE::ATTACK_STATE_ENUM::kBowDraw || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowReleased || attackState == RE::ATTACK_STATE_ENUM::kNextAttack || attackState == RE::ATTACK_STATE_ENUM::kBowNextAttack;
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
	if (auto CrosshairPickData = RE::CrosshairPickData::GetSingleton(); CrosshairPickData)
		if (auto refr = CrosshairPickData->target.get(); refr)
			return refr->GetFormType() != RE::FormType::ActorCharacter || refr.get()->As<RE::Actor>()->IsDead();
	return false;
}

void HUDManager::UpdateHUD(RE::PlayerCharacter* player, double detectionLevel, RE::GFxValue sneakAnim)
{
	auto SmoothCam = SmoothCamCompat();
	auto TDM = TDMCompat();
	auto DetectionMeter = DetectionMeterCompat();
	auto BTPS = BTPSCompat();

	fadeMult = 1.0;

	auto fadeIn = !(SmoothCam || TDM) && (ValidCastType(player->magicCasters[0]) || ValidCastType(player->magicCasters[1]) || ValidAttackType(player));
	
	fadeMult = std::lerp(prevFadeMult, fadeIn ? fadeMult : fadeMult * 8, prevDelta / fadeMult);
	prevFadeMult = fadeMult;

	alpha = std::lerp(alpha, (fadeIn || (ValidPickType() && !BTPS)) ? (!TDM && (fadeIn || SmoothCam) ? 100.0 : 50.0) : 0.0, prevDelta * fadeMult);

	if (player->IsSneaking()) {
		sneakAlpha = std::lerp(sneakAlpha, !(SmoothCam || TDM) ? std::clamp(DetectionMeter ? alpha : detectionLevel + alpha, 0.0, 100.0) : DetectionMeter ? 0.0 : detectionLevel / 2, prevDelta * fadeMult);
	} else {
		sneakAlpha = std::lerp(sneakAlpha, 0.0, prevDelta * 16);
	}

	UpdateCrosshair();
	UpdateStealthAnim(sneakAnim);
}
