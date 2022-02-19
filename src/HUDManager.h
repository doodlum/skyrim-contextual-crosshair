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

	void UpdateHUD(RE::PlayerCharacter* player, float delta);

	static inline double maxOpacity = 100;
	static inline double fadeSpeed = 1.0; // seconds
	static inline double alpha = 0.0;


protected:

	struct Hooks
	{
		struct PlayerCharacter_Update
		{
			static void thunk(RE::PlayerCharacter* player, float delta)
			{
				func(player, delta);
	
				SKSE::GetTaskInterface()->AddUITask([player, delta]() {
					GetSingleton()->UpdateHUD(player, delta);
				});
			}
			static inline REL::Relocation<decltype(&thunk)> func;
		};

		static void Install()
		{
			stl::write_vfunc<RE::PlayerCharacter, 0xAD, PlayerCharacter_Update>();
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
