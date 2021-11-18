#pragma once

#include <vector>

#include "Action.hpp"
#include "CraftInfo.hpp"
#include "State.hpp"
#include "CrafterCore.hpp"

class Finisher
{
public:
	int cp = 0;
	int durability = 0;
	int progress = 0;
	int quality = 0;

	const std::vector<Action> actions;

	Finisher(const CraftInfo info, const std::vector<Action> actions);
};