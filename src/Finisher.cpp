#include "Finisher.hpp"

Finisher::Finisher(const CraftInfo info, const std::vector<Action> actions) : actions(actions)
{
	auto first_state = State(info);
	first_state.buff.at(StatusEffect::初手) = 0;
	first_state.inner_quiet = 10;
	auto last_state = first_state;
	for (auto action : actions)
	{
		if (last_state.isEnd(info))
		{
			throw "Finisher Build Error : Unexpected End";
		}
		CrafterCore::DeterministicExecuteAction(info, &last_state, action, Condition::通常, true);
	}

	this->cp = first_state.cp - last_state.cp;
	this->durability = first_state.durability - last_state.durability + CrafterCore::ALL_ACTIONS[actions.back()].durability + 1;
	this->progress = info.max_progress - last_state.progress;
	this->quality = info.max_quality - last_state.quality;
}
