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
	std::map<StatusEffect, int> buff = {
		{StatusEffect::初手, 0},
		{StatusEffect::グレートストライド, 0},
		{StatusEffect::イノベーション, 0},
		{StatusEffect::マニピュレーション, 0},
		{StatusEffect::確信, 0},
		{StatusEffect::倹約, 0},
		{StatusEffect::長期倹約, 0},
		{StatusEffect::経過観察, 0},
		{StatusEffect::最終確認, 0},
		{StatusEffect::アートオブエレメンタル, 0},
		{StatusEffect::ヴェネレーション, 0},
		{StatusEffect::加工, 0}};

	int 設計変更Count = 0;
	int 一心不乱Count = 0;
	int inner_quiet = 0;

	State() {}

	State(const CraftInfo &info) : cp(info.max_cp),
								   durability(info.max_durability)
	{
		this->buff.at(StatusEffect::初手) = 1;
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