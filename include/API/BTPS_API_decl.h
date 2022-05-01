#pragma once
#include <functional>
#include <stdint.h>

class BTPS_API_decl
{
public:
    static constexpr const char* pluginName = "BetterThirdPersonSelection.dll";

    enum class Version
    {
        V0,
    };

    // -- interface version declarations
    class API_V0
    {
    public:
        virtual bool GetSelectionEnabled() noexcept = 0;
        virtual bool GetWidget3DEnabled() noexcept = 0;

		// as source, use anything unique to your plugin, like your plugin name
		// caution: don't use menuname such as 'HUD Menu' etc. or things might go wrong
        virtual void HideSelectionWidget(std::string source) noexcept = 0;
        virtual void ShowSelectionWidget(std::string source) noexcept = 0;
    };

    // --

private:
    typedef void* (*_RequestPluginAPI)(const Version apiVersion);

    // -- interface requests
private:
    static [[nodiscard]] inline void* RequestPluginAPI(const Version apiVersion = Version::V0)
    {
        HMODULE pluginHandle = GetModuleHandleA(pluginName);
        if (!pluginHandle)
            return nullptr;

        _RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
        if (requestAPIFunction)
            return requestAPIFunction(apiVersion);

        return nullptr;
    }

public:
	// call this in kPostLoad or similar to get access to the BTPS API
    static [[nodiscard]] inline API_V0* RequestPluginAPI_V0()
    {
        return reinterpret_cast<API_V0*>(RequestPluginAPI(Version::V0));
    }
};
