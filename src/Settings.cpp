#pragma once

#include "SimpleIni.h"

class Settings
{
public:
	struct Crosshair
	{
		int mode{ 0 };
		double min{ 0.00 };
		double max{ 100.00 };
	};

	struct SneakMeter
	{
		int mode{ 0 };
		double min{ 0.00 };
		double max{ 90.00 };
		double range{ 0.1 };
		double frequency{ 0.2 };
	};

	struct Spells
	{
		int mode{ 0 };
	};

	struct Fade
	{
		double time{ 1.00 };
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
		crosshair.min = ini.GetDoubleValue("Crosshair", "OpacityMin", 0.0);
		crosshair.max = ini.GetDoubleValue("Crosshair", "OpacityMax", 100.0);

		sneakMeter.mode = ini.GetLongValue("SneakMeter", "Mode");
		sneakMeter.min = ini.GetDoubleValue("SneakMeter", "OpacityMin", 0.0);
		sneakMeter.max = ini.GetDoubleValue("SneakMeter", "OpacityMax", 90.0);
		sneakMeter.range = ini.GetDoubleValue("SneakMeter", "PulseRange", 0.05);
		sneakMeter.frequency = ini.GetDoubleValue("SneakMeter", "PulseFrequency", 0.05);

		spells.mode = ini.GetLongValue("Spells", "Mode");

		fade.time = ini.GetDoubleValue("Fade", "Multiplier", 1.0);

		smoothCam.mode = ini.GetLongValue("SmoothCam", "Mode");
	}

	[[nodiscard]] int GetCrosshairMode() const { return crosshair.mode; }

	[[nodiscard]] int GetSneakMeterMode() const { return sneakMeter.mode; }

	[[nodiscard]] int GetSpellsMode() const { return spells.mode; };

	[[nodiscard]] double GetFadeMultiplier() const { return fade.time; }

	[[nodiscard]] double GetCrosshairMinOpacity() const { return crosshair.min; }
	[[nodiscard]] double GetCrosshairMaxOpacity() const { return crosshair.max; }

	[[nodiscard]] double GetSneakMeterMinOpacity() const { return sneakMeter.min; }
	[[nodiscard]] double GetSneakMeterMaxOpacity() const { return sneakMeter.max; }

	[[nodiscard]] double GetSneakMeterRange() const { return sneakMeter.range; }
	[[nodiscard]] double GetSneakMeterFrequency() const { return sneakMeter.frequency; }

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
	SmoothCam  smoothCam;
};
