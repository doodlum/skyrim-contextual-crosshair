#pragma once

#include "SimpleIni.h"

class Settings
{
public:
	struct Crosshair
	{
		int mode{ 0 };
	};

	struct SneakMeter
	{
		int mode{ 0 };
	};

	struct Spells
	{
		int mode{ 0 };
	};

	struct Fade
	{
		double time{ 1.00 };
	};

	struct Opacity
	{
		double min{ 0.00 };
		double max{ 100.00 };
	};

	struct SmoothCam
	{
		int mode{ 0 };
	};

	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}


	void LoadSettings()
	{
		constexpr auto path = L"Data/SKSE/Plugins/ContextualCrosshair.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		crosshair.mode = ini.GetLongValue("Crosshair", "Mode");

		sneakMeter.mode = ini.GetLongValue("SneakMeter", "Mode");

		spells.mode = ini.GetLongValue("Spells", "Mode");

		fade.time = ini.GetDoubleValue("Fade", "Multiplier", 1.0);

		opacity.min = ini.GetDoubleValue("Opacity", "Min", 0.0);
		opacity.max = ini.GetDoubleValue("Opacity", "Max", 100.0);

		smoothCam.mode = ini.GetLongValue("SmoothCam", "Mode");
	}

	[[nodiscard]] int GetCrosshairMode() const { return crosshair.mode; }

	[[nodiscard]] int GetSneakMeterMode() const { return sneakMeter.mode; }

	[[nodiscard]] int GetSpellsMode() const { return spells.mode; };

	[[nodiscard]] double GetFadeMultiplier() const { return fade.time; }

	[[nodiscard]] double GetMinOpacity() const { return opacity.min; }
	[[nodiscard]] double GetMaxOpacity() const { return opacity.max; }

	[[nodiscard]] int GetSmoothCamMode() const { return smoothCam.mode; };

private:
	struct detail
	{
		static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
			a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);
		}
	};

	Settings() = default;
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	~Settings() = default;

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;

	Crosshair  crosshair;
	SneakMeter sneakMeter;
	Spells     spells;
	Fade       fade;
	Opacity    opacity;
	SmoothCam  smoothCam;
};
