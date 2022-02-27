
#include "Hooks.h"
#include "HUDManager.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
		case SKSE::MessagingInterface::kPostLoad:
			if (!SmoothCamAPI::RegisterInterfaceLoaderCallback(SKSE::GetMessagingInterface(),
					[](void* interfaceInstance, SmoothCamAPI::InterfaceVersion interfaceVersion) {
						if (interfaceVersion == SmoothCamAPI::InterfaceVersion::V3) {
							HUDManager::GetSingleton()->g_SmoothCam = reinterpret_cast<SmoothCamAPI::IVSmoothCam3*>(interfaceInstance);
							logger::info("Obtained SmoothCamAPI");
						} else {
							logger::error("Unable to acquire requested SmoothCamAPI interface version");
						}
					})) {
				logger::warn("SmoothCamAPI::RegisterInterfaceLoaderCallback reported an error");
			}

			if (!TDM_API::RegisterInterfaceLoaderCallback(SKSE::GetMessagingInterface(),
					[](void* interfaceInstance, TDM_API::InterfaceVersion interfaceVersion) {
						if (interfaceVersion == TDM_API::InterfaceVersion::V1) {
							HUDManager::GetSingleton()->g_TDM = reinterpret_cast<TDM_API::IVTDM1*>(interfaceInstance);
							logger::info("Obtained TDM API");
						} else {
							logger::error("Unable to acquire requested TDM API interface version");
						}
					})) {
				logger::warn("TDM_API::RegisterInterfaceLoaderCallback reported an error");
			}

			if (HUDManager::GetSingleton()->g_DetectionMeter = LoadLibrary(".\\Data\\SKSE\\Plugins\\MaxsuDetectionMeter.dll"))
				logger::info("Obtained Detection Meter DLL");
			else
				logger::info("Unable to acquire Detection Meter DLL");

			break;

		case SKSE::MessagingInterface::kPostPostLoad:
			if (!SmoothCamAPI::RequestInterface(
					SKSE::GetMessagingInterface(),
					SmoothCamAPI::InterfaceVersion::V3))
				logger::warn("SmoothCamAPI::RequestInterface reported an error");
			if (!TDM_API::RequestInterface(
					SKSE::GetMessagingInterface(),
					TDM_API::InterfaceVersion::V1))
				logger::warn("TDMAPI::RequestInterface reported an error");
			break;
	}
}


void Init()
{
	SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);
	Hooks::Install();
}

void InitializeLog()
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		util::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format("{}.log"sv, Plugin::NAME);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
	const auto level = spdlog::level::trace;
#else
	const auto level = spdlog::level::info;
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {};
#endif

#ifdef SKYRIMAE
	InitializeLog();
#endif

	logger::info("Loaded plugin");

	SKSE::Init(a_skse);

	Init();

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	InitializeLog();

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION.pack();

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver != RUNTIME) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"sv), ver.string());
		return false;
	}

	return true;
}

#ifdef SKYRIMAE
extern "C" DLLEXPORT constinit SKSE::PluginVersionData SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.UsesAddressLibrary(true);

	return v;
}();
#endif
