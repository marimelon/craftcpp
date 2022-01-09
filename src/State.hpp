#pragma once

#include <array>
#include <map>
#include <set>
#include <algorithm>

#include "magic_enum.hpp"
#include "cexpr_map.hpp"

#include "Condition.hpp"
#include "StatusEffect.hpp"
#include "CraftInfo.hpp"

struct State
{
	int cp = 0;
	int progress = 0;
	int quality = 0;
	int durability = 0;
	Condition condition = Condition::通常;
	int turn = 0;
	std::map<StatusEffect, int> buff = {};

	int 設計変更Count = 0;
	int 一心不乱Count = 0;
	int inner_quiet = 0;

	State() : State(CraftInfo()) {}

	State(const CraftInfo &info) : cp(info.max_cp),
								   durability(info.max_durability)
	{
		for (auto val : magic_enum::enum_values<StatusEffect>())
		{
			this->buff.emplace(val, 0);
		}

		this->buff.at(StatusEffect::初手) = 1;
	}

	bool operator==(const State &rhs) const
	{
		return this->cp == rhs.cp &&
			   this->progress == rhs.progress &&
			   this->quality == rhs.quality &&
			   this->durability == rhs.durability &&
			   this->condition == rhs.condition &&
			   this->turn == rhs.turn &&
			   std::equal(
				   std::begin(this->buff),
				   std::end(this->buff),
				   std::begin(rhs.buff),
				   std::end(rhs.buff)) &&
			   this->設計変更Count == rhs.設計変更Count &&
			   this->一心不乱Count == rhs.一心不乱Count &&
			   this->inner_quiet == rhs.inner_quiet;
	}

	inline bool operator!=(const State &rhs) const
	{
		return !(*this == rhs);
	}

	bool isEnd(const CraftInfo &info)
	{
		return (this->progress >= info.max_progress || this->durability <= 0);
	}

	float getScore(const CraftInfo &info)
	{
		if (this->progress >= info.max_progress)
		{
			return this->quality / (float)info.max_quality;
		}
		else
		{
			return 0.;
		}
	}

	float getScore2(const CraftInfo &info)
	{
		if (this->progress >= info.max_progress)
		{
			return this->quality / (float)info.max_quality;
		}
		else
		{
			return ((this->progress / (float)info.max_progress) + (this->quality / (float)info.max_quality) - 2) / 2.f;
		}
	}

	float getScore3(const CraftInfo &info)
	{
		return ((this->progress / (float)info.max_progress) + (this->quality / (float)info.max_quality)) - 1;
	}

	std::string to_string();

	std::set<Action> legal_actions(const CraftInfo &info);
};