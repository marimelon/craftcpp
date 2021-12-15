#include <iostream>
#include <thread>

#include "magic_enum.hpp"

#include "State.hpp"
#include "StatusEffect.hpp"
#include "CrafterCore.hpp"
#include "CraftInfo.hpp"
#include "Finisher.hpp"

using AC = Action;

int main()
{
	auto craft_status = CraftInfo();
	craft_status.max_cp = 569;
	craft_status.max_durability = 80;
	craft_status.max_progress = 3467;
	craft_status.max_quality = 14902;
	craft_status.base_progress = 927; //作業効率100に対する作業増加値
	craft_status.base_quality = 647;
	craft_status.illegal_actions = {
		AC::ヘイスティタッチ,
		AC::匠の早業, AC::最終確認, AC::注視作業, AC::注視加工, AC::突貫作業, AC::経過観察, AC::設計変更};

	craft_status.condition_rates[Condition::長持続] = 1;

	std::string aa = "初手";
	auto s = magic_enum::enum_type_name<StatusEffect>();

	for (auto sf : magic_enum::enum_values<StatusEffect>())
	{
		auto name = magic_enum::enum_name<StatusEffect>(sf);
		auto name_s = std::string(name);
		auto name_c = name_s.c_str();
		std::cout << name << std::endl;
	}

	std::vector<Condition> results{};
	for (int i = 0; i < 10; i++)
	{
		auto result = CrafterCore::RandomlyGenNextCondition(craft_status, Condition::通常);
		results.push_back(result);
	}

	auto name = typeid(CrafterCore::ALL_ACTIONS).name();

	auto m = CrafterCore::ALL_ACTIONS.gen_map();

	auto s1 = State(craft_status);
	auto s2 = s1;

	s1.buff.at(StatusEffect::初手) = 0;

	// for (int i = 0; i < 1000000; i++) {
	// 	CrafterCore::ExecuteAction(CraftStatus(), states.back(), Action::真価);
	// }
	//auto result = CrafterCore::AvailableActions(craft_status, states.back());

	std::vector<State> states{State(craft_status)};
	states.push_back(CrafterCore::ExecuteAction(craft_status, states.back(), Action::真価));
	states.push_back(CrafterCore::ExecuteAction(craft_status, states.back(), Action::グレートストライド));
	states.push_back(CrafterCore::ExecuteAction(craft_status, states.back(), Action::加工));
	states.back().progress = craft_status.max_progress;
	auto score = states.back().getScore(craft_status);
	auto str = states.back().to_string();
	std::cout << str << std::endl;
	auto finisher = Finisher(craft_status, {Action::加工, Action::作業});
	return 0;
}