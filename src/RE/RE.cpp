#include "RE.h"

namespace RE
{
	bool IsCasting(Actor* a_actor, SpellItem* a_spell)
	{
		using func_t = decltype(&IsCasting);
		REL::Relocation<func_t> func{ REL::ID(37810) };
		return func(a_actor, a_spell);
	}
}
