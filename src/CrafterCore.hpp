#pragma once

#include<unordered_map>
#include <numeric>

#include "Action.hpp"
#include "State.hpp"
#include "StatusEffect.hpp"
#include "CraftInfo.hpp"
#include "cexpr_map.hpp"

struct ActionParams {
	int cp;
	int durability;
	int rate;
	int progress;
	int quality;
};

class CrafterCore
{
public:
	static constexpr auto ALL_ACTIONS = make_map<Action, ActionParams>({
		{Action::作業,{0, -10, 100, 120, 0}},
		{Action::加工,{-18, -10, 100, 0, 100}},
		{Action::マスターズメンド,{-88, 30, 100, 0, 0}},
		{Action::ヘイスティタッチ,{0, -10, 60, 0, 100}},
		{Action::突貫作業,{0, -10, 50, 500, 0}},
		{Action::インナークワイエット,{-18, 0, 100, 0, 0}},
		{Action::経過観察,{-7, 0, 100, 0, 0}},
		{Action::秘訣,{20, 0, 100, 0, 0}},
		{Action::倹約,{-56, 0, 100, 0, 0}},
		{Action::ヴェネレーション,{-18, 0, 100, 0, 0}},
		{Action::中級加工,{-32, -10, 100, 0, 125}},
		{Action::グレートストライド,{-32, 0, 100, 0, 0}},
		{Action::イノベーション,{-18, 0, 100, 0, 0}},
		{Action::アートオブエレメンタル,{-30, 0, 100, 0, 0}},
		{Action::ブランドオブエレメンタル,{-6, -10, 100, 100, 0}},
		{Action::最終確認,{-1, 0, 100, 0, 0}},
		{Action::長期倹約,{-98, 0, 100, 0, 0}},
		{Action::ビエルゴの祝福,{-24, -10, 100, 0, 100}},
		{Action::集中加工,{-18, -10, 100, 0, 150}},
		{Action::確信,{-6, 0, 100, 300, 0}},
		{Action::模範作業,{-7, -10, 100, 150, 0}},
		{Action::専心加工,{-6, -10, 50, 0, 100}},
		{Action::マニピュレーション,{-96, 0, 100, 0, 0}},
		{Action::倹約加工,{-25, -5, 100, 0, 100}},
		{Action::注視作業,{-5, -10, 50, 200, 0}},
		{Action::注視加工,{-18, -10, 50, 0, 150}},
		{Action::真価,{-24, 0, 100, 0, 100}},
		{Action::下地加工,{-40, -20, 100, 0, 200}},
		{Action::下地作業,{-18, -20, 100, 200, 0}},
		{Action::精密作業,{-32, -10, 100, 100, 100}},
		{Action::集中作業,{-6, -10, 100, 300, 0}},
		{Action::匠の早業,{-250, -10, 100, 0, 0}},
		{Action::設計変更,{0, 0, 100, 0, 0}},
		});

	static bool CanExecuteAction(const CraftInfo& craft_status, const State& state, const Action& action);
	static void ApplyCPDurabilityChange(const CraftInfo& craft_status, State* state, const Action& action);
	static void ApplyProgressChange(const CraftInfo& craft_status, State* state, const Action& action);
	static void ApplyQualityChange(const CraftInfo& craft_status, State* state, const Action& action);
	static void ApplyInnerQuietChange(const CraftInfo& craft_status, State* state, const Action& action, bool is_action_successful);
	static void ApplyPersistentBuffEffect(const CraftInfo& craft_status, State* state, const Action& action);
	static void ApplyBuffChange(const CraftInfo& craft_status, State* state, const Action& action);
	static void DeterministicExecuteAction(const CraftInfo& craft_status, State* state, const Action& action, const Condition next_condition, bool is_action_successful);

	static Condition RandomlyGenNextCondition(const CraftInfo& craft_status,const Condition condition);
	static std::vector<Action> AvailableActions(const CraftInfo& craft_status, const State& state);
	static State ExecuteAction(const CraftInfo& craft_status, State state, Action action);
};

