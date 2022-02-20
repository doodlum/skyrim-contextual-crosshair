#pragma once

class HUDManager
{
public:

	[[nodiscard]] static HUDManager* GetSingleton()
	{
		static HUDManager singleton;
		return std::addressof(singleton);
	}

	static void InstallHooks()
	{
		Hooks::Install();
	}

	void UpdateCrosshair(RE::PlayerCharacter* player);
	void UpdateStealthAnim(RE::PlayerCharacter* player, RE::GFxValue stealthAnim, double detectionLevel);
	void UpdateHUD(RE::PlayerCharacter* player, RE::GFxValue stealthAnim, double detectionLevel);

	static inline double maxOpacity = 100;
	static inline double fadeSpeed = 1.0; // seconds
	static inline double alpha = 0.0;
	static inline double sneakAlpha = 0.0;
	static inline double prevDelta = 0.0;

protected:

	struct Hooks
	{
		struct PlayerCharacter_Update
		{
			static void thunk(RE::PlayerCharacter* player, float delta)
			{
				func(player, delta);

				prevDelta = delta;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct StealthMeter_Update
		{
			static char thunk(RE::StealthMeter* a1, int64_t a2, int64_t a3, int64_t a4)
			{
				auto result = func(a1, a2, a3, a4);

				auto detectionLevel = static_cast<double>(a1->unk88);
				auto stealthCrosshair = a1->sneakAnim;

				auto player = RE::PlayerCharacter::GetSingleton();
				if (player) {
					SKSE::GetTaskInterface()->AddUITask([player, stealthCrosshair, detectionLevel]() {
						GetSingleton()->UpdateHUD(player, stealthCrosshair, detectionLevel);
					});
				}
				return result;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install()
		{
			auto hehptr = RE::VTABLE_StealthMeter[0];
			stl::write_vfunc<RE::PlayerCharacter, 0xAD, PlayerCharacter_Update>();
			stl::write_vfunc<0x1, StealthMeter_Update>(RE::VTABLE_StealthMeter[0]);
		}
	};


private:
	constexpr HUDManager() noexcept = default;
	HUDManager(const HUDManager&) = delete;
	HUDManager(HUDManager&&) = delete;

	~HUDManager() = default;

	HUDManager& operator=(const HUDManager&) = delete;
	HUDManager& operator=(HUDManager&&) = delete;
};
