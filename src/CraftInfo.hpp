#pragma once

#include <array>
#include <vector>
#include <optional>
#include <map>

#include "Action.hpp"
#include "Condition.hpp"

struct CraftInfo
{
	int max_cp = 0;
	int max_durability = 0;
	int max_progress = 0;
	int max_quality = 0;
	int base_progress = 0; //作業効率100に対する作業増加値
	std::array<int, 12> iq_table{ 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0 };
	std::vector<Action> illegal_actions = {};
	std::map<Condition, float> condition_rates = {
		{Condition::通常 ,1.},
		{Condition::高品質,0.},
		{Condition::最高品質,0.},
		{Condition::低品質,0.},
		{Condition::安定,0.},
		{Condition::頑丈,0.},
		{Condition::高能率,0.},
		{Condition::高進捗,0.},
		{Condition::長持続,0.},
	};

	auto set(
		std::optional<int> max_cp,
		std::optional<int> max_durability,
		std::optional<int> max_progress,
		std::optional<int> max_quality,
		std::optional<int> base_progress,
		std::optional<std::array<int, 12>> iq_table,
		std::optional<std::vector<Action>> illegal_actions,
		std::optional<std::map<Condition, float>> condition_rates)
	{
		if (max_cp.has_value()) {
			this->max_cp = max_cp.value();
		}
		if (max_durability.has_value()) {
			this->max_durability = max_durability.value();
		}
		if (max_progress.has_value()) {
			this->max_progress = max_progress.value();
		}
		if (max_quality.has_value()) {
			this->max_quality = max_quality.value();
		}
		if (base_progress.has_value()) {
			this->base_progress = base_progress.value();
		}
		if (iq_table.has_value()) {
			this->iq_table = iq_table.value();
		}
		if (illegal_actions.has_value()) {
			this->illegal_actions = illegal_actions.value();
		}
		if (condition_rates.has_value()) {
			this->condition_rates = condition_rates.value();
		}

		return this;
	}
};