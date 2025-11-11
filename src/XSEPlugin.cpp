#include "Hooks.h"
#include "HUDManager.h"

#include "Settings.cpp"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		if (!SmoothCamAPI::RegisterInterfaceLoaderCallback(SKSE::GetMessagingInterface(),
				[](void* interfaceInstance, SmoothCamAPI::InterfaceVersion interfaceVersion) {
					if (interfaceVersion == SmoothCamAPI::InterfaceVersion::V3) {
						HUDManager::GetSingleton()->g_SmoothCam = reinterpret_cast<SmoothCamAPI::IVSmoothCam3*>(interfaceInstance);
						logger::info("Obtained SmoothCam API");
					} else {
						logger::error("Unable to acquire requested SmoothCam API interface version");
					}
				})) {
			logger::warn("SmoothCamAPI::RegisterInterfaceLoaderCallback reported an error");
		}

		HUDManager::GetSingleton()->g_TDM = reinterpret_cast<TDM_API::IVTDM2*>(TDM_API::RequestPluginAPI(TDM_API::InterfaceVersion::V2));
		if (HUDManager::GetSingleton()->g_TDM)
			logger::info("Obtained TDM API");
		else
			logger::info("Unable to acquire TDM API");

		HUDManager::GetSingleton()->g_BTPS = reinterpret_cast<BTPS_API_decl::API_V0*>(BTPS_API_decl::RequestPluginAPI_V0());
		if (HUDManager::GetSingleton()->g_BTPS)
			logger::info("Obtained BTPS API");
		else
			logger::info("Unable to acquire BTPS API");

		HUDManager::GetSingleton()->g_DetectionMeter = LoadLibraryA("Data/SKSE/Plugins/MaxsuDetectionMeter.dll");
		if (HUDManager::GetSingleton()->g_DetectionMeter)
			logger::info("Obtained Detection Meter DLL");
		else
			logger::info("Unable to acquire Detection Meter DLL");

		break;


	case SKSE::MessagingInterface::kPostPostLoad:
		if (!SmoothCamAPI::RequestInterface(
				SKSE::GetMessagingInterface(),
				SmoothCamAPI::InterfaceVersion::V3))
			logger::warn("SmoothCamAPI::RequestInterface reported an error");
		break;

	case SKSE::MessagingInterface::kPreLoadGame:
		Settings::GetSingleton()->LoadSettings();
		break;

	case SKSE::MessagingInterface::kDataLoaded:
		if (HUDManager::GetSingleton()->g_SmoothCam)  // SmoothCam is present
		{
			HUDManager::GetSingleton()->InitIFPV();
			if (HUDManager::GetSingleton()->g_IFPV)
				logger::info("IFPV Detected - SmoothCam Compatibility Enabled");
		}
		break;
	}
}


void Load()
{
	SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);
	Hooks::Install();
	Settings::GetSingleton()->LoadSettings();
}
