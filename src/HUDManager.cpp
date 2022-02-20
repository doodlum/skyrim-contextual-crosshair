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
		auto spell = form->As<RE::SpellItem>();
		return IsCasting(player, spell) && (spell->GetDelivery() == RE::MagicSystem::Delivery::kAimed) && (spell->GetCastingType() != RE::MagicSystem::CastingType::kConcentration);
	}
	return false;
}

bool ValidStaffType(RE::TESForm* form, RE::ActorMagicCaster* magicCaster)
{
	if (form && form->Is(RE::FormType::Weapon)) {
		auto weapon = form->As<RE::TESObjectWEAP>();
		if (weapon->IsStaff()) {
			RE::EnchantmentItem* enchantment = weapon->formEnchanting;
			if (enchantment) {
				return (magicCaster->currentSpell) && (enchantment->GetDelivery() == RE::MagicSystem::Delivery::kAimed) && (enchantment->GetCastingType() != RE::MagicSystem::CastingType::kConcentration);
			}
		}
	}
	return false;
}

bool ValidAttackType(RE::PlayerCharacter* player)
{
	auto attackState = player->actorState1.meleeAttackState;
	return attackState == RE::ATTACK_STATE_ENUM::kBowAttached || attackState == RE::ATTACK_STATE_ENUM::kBowDrawn;
}

void HUDManager::UpdateCrosshair(RE::PlayerCharacter* player)
{
	auto pickData = RE::CrosshairPickData::GetSingleton()->target;
	auto leftHand = player->GetEquippedObject(true);
	auto rightHand = player->GetEquippedObject(false);
	auto leftHandCaster = player->magicCasters[0];
	auto rightHandCaster = player->magicCasters[1];

	auto isVisible = pickData || ValidStaffType(leftHand, leftHandCaster) || ValidStaffType(rightHand, rightHandCaster) || ValidAttackType(player) || ValidCastType(player, leftHand) || ValidCastType(player, rightHand);

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
