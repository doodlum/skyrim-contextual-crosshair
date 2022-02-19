#include "Hooks.h"
#include "HUDManager.h"

namespace Hooks
{
	void Hooks::Install()
	{
		HUDManager::InstallHooks();
		logger::info("Installed all hooks");
	}
}
