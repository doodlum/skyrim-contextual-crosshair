#include "Hooks.h"
#include "ConditionalHUDDisplay.h"

namespace Hooks
{
	void Hooks::Install()
	{
		ConditionalHUDDisplay::InstallHooks();
		logger::info("Installed all hooks");
	}
}
