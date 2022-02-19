#include "HUDManager.h"

using namespace std;
using WEAPON_STATE = RE::WEAPON_STATE;
using ATTACK_STATE = RE::ATTACK_STATE_ENUM;
using PLAYER_ACTION = RE::PLAYER_ACTION;

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


bool IsCasting(RE::Actor* actor, RE::SpellItem* a_spell)
{
	using func_t = decltype(&IsCasting);
	REL::Relocation<func_t> func{ REL::ID(37810) };
	return func(actor, a_spell);
}


bool ValidCastType(RE::PlayerCharacter* player, RE::TESForm* form)
{
	if (form->Is(RE::FormType::Spell) ){
		auto spell = bit_cast<RE::SpellItem*>(form);
		return IsCasting(player, spell) && (spell->GetDelivery() == RE::MagicSystem::Delivery::kAimed);
	}
	return false;
}


void HUDManager::UpdateHUD(RE::PlayerCharacter* player, float delta)
{
	auto attackState = player->actorState1.meleeAttackState;
	auto pickData = RE::CrosshairPickData::GetSingleton()->target;

	auto left = player->GetEquippedObject(true);
	auto right = player->GetEquippedObject(false);

	bool isVisible = 
		pickData || 
		ValidCastType(player, left) || 
		ValidCastType(player, right) || 
		attackState == ATTACK_STATE::kBowAttached || 
		attackState == ATTACK_STATE::kBowDrawn;

	auto crosshair = GetGFxValue("_root.HUDMovieBaseInstance.CrosshairInstance");
	if (crosshair != nullptr) {
		RE::GFxValue::DisplayInfo displayInfo;
		crosshair.GetDisplayInfo(addressof(displayInfo));

		if (isVisible) {
			alpha += delta * (maxOpacity / fadeSpeed);
		} else {
			alpha -= delta * (maxOpacity / fadeSpeed) * 3;
		}
		alpha = clamp(alpha, 0.0, maxOpacity);

		displayInfo.SetAlpha(alpha);
		crosshair.SetDisplayInfo(displayInfo);
	}
}
