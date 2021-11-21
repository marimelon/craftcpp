#include "CrafterCore.hpp"
#include "random.hpp"

using SF = StatusEffect;

bool CrafterCore::CanExecuteAction(const CraftInfo& craft_status, const State& state, const Action& action)
{
	// 使用可能なアクションか判定する

	if (!std::none_of(craft_status.illegal_actions.begin(), craft_status.illegal_actions.end(), [action](Action x) { return x == action; })) {
		return false;
	}

	if (state.durability == 0 || state.progress >= craft_status.max_progress) {
		return false;
	}

	auto d_cp = ALL_ACTIONS[action].cp;
	if (state.condition == Condition::高能率) {
		d_cp = std::floor(d_cp / 2.0);
	}
	if (state.cp + d_cp < 0) {
		return false;
	}

	switch (action)
	{
	case Action::インナークワイエット:
		return state.inner_quiet == 0;
	case Action::秘訣:
	case Action::集中加工:
	case Action::集中作業:
		return state.condition == Condition::高品質 || state.condition == Condition::最高品質;
	case Action::倹約加工:
		return state.buff.at(StatusEffect::倹約) == 0 && state.buff.at(StatusEffect::長期倹約) == 0;
	case Action::ビエルゴの祝福:
		return state.inner_quiet > 1;
	case Action::真価:
	case Action::確信:
		return state.buff.at(StatusEffect::初手) != 0;
	case Action::設計変更:
		return state.設計変更Count < 3;
	}

	return true;
}

void CrafterCore::ApplyCPDurabilityChange(const CraftInfo& craft_status, State* state, const Action& action)
{
	// CP,耐久度の増減を処理する

	auto action_parm = ALL_ACTIONS[action];
	// CP
	auto d_cp = action_parm.cp;
	if (state->condition == Condition::高能率) {
		d_cp = static_cast<int>(std::floor(d_cp / 2.));
	}

	if(state->buff.at(SF::加工) > 0 && action == Action::中級加工){
		d_cp = -18;
	}

	state->cp = std::min(craft_status.max_cp, state->cp + d_cp);

	// Durability
	auto d_du = action_parm.durability;

	if (d_du < 0 && state->condition == Condition::頑丈) {
		d_du = static_cast<int>(std::floor(d_du / 2.));
	}

	if (d_du < 0 && (state->buff.at(SF::倹約) > 0 || state->buff.at(SF::長期倹約) > 0)) {
		d_du = static_cast<int>(std::floor(d_du / 2.));
	}
	state->durability = std::min(craft_status.max_durability, state->durability + d_du);
}

void CrafterCore::ApplyProgressChange(const CraftInfo& craft_status, State* state, const Action& action)
{
	// 工数を処理する

	auto eff = ALL_ACTIONS[action].progress;
	if (eff == 0) {
		return;
	}

	eff /= 100;

	if (state->buff.at(SF::確信) > 0) {
		eff *= 2.;
		state->buff.at(SF::確信) = 0;
	}
	if (state->buff.at(SF::ヴェネレーション) > 0) {
		eff *= 1.5;
	}

	if (state->condition == Condition::高進捗) {
		eff *= 1.5;
	}

	state->progress += std::floor(craft_status.base_progress * eff);

	if (state->progress < craft_status.max_progress) {
		return;
	}

	if (state->buff.at(SF::最終確認) > 0) {
		state->progress = craft_status.max_progress - 1;
		state->buff.at(SF::最終確認) = 0;
	}
	else {
		state->progress = craft_status.max_progress;
	}
}

void CrafterCore::ApplyQualityChange(const CraftInfo& craft_status, State* state, const Action& action)
{
	// 品質を処理する

	auto eff = ALL_ACTIONS[action].quality;
	if (eff == 0) {
		return;
	}

	if (action == Action::ビエルゴの祝福) {
		if (state->inner_quiet < 2) {
			throw "インナークワイエット2以上必要";
		}
		eff = 1. + .2 * (state->inner_quiet - 1);
	}
	else {
		eff = eff / 100.;
	}

	auto buff_multiplier = 1.;

	if (state->buff.at(SF::グレートストライド)) {
		buff_multiplier += 1.;
		state->buff.at(SF::グレートストライド) = 0;
	}

	if (state->buff.at(SF::イノベーション)) {
		buff_multiplier += 0.5;
	}

	switch (state->condition)
	{
	case Condition::高品質:
		eff *= 2.;
		break;
	case Condition::最高品質:
		eff *= 4;
		break;
	case Condition::低品質:
		eff *= 0.5;
		break;
	default:
		break;
	}

	//d = math.floor(self.iq_table[self.inner_quiet]*eff*buff_multiplier)
	state->quality += std::floor(craft_status.iq_table[state->inner_quiet] * eff * buff_multiplier);
}

void CrafterCore::ApplyInnerQuietChange(const CraftInfo& craft_status, State* state, const Action& action, bool is_action_successful)
{
	// インナークワイエットを処理する

	switch (action)
	{
	case Action::加工:
	case Action::ヘイスティタッチ:
	case Action::中級加工:
	case Action::倹約加工:
	case Action::注視加工:
	case Action::精密作業:
		state->inner_quiet += 1;
		break;
	case Action::ビエルゴの祝福:
		state->inner_quiet = 0;
		break;
	case Action::集中加工:
	case Action::下地加工:
		state->inner_quiet += 2;
		break;
	case Action::専心加工:
		if (is_action_successful) {
			state->inner_quiet *= 2;
		}
		else {
			state->inner_quiet = std::ceil(state->inner_quiet / 2.);
		}
		break;
	case Action::真価:
		state->inner_quiet = 3;
		break;
	}
	state->inner_quiet = std::min(11, state->inner_quiet);
}

void CrafterCore::ApplyPersistentBuffEffect(const CraftInfo& craft_status, State* state, const Action& action)
{
	// バフのターンを進める

	if (state->buff.at(SF::マニピュレーション) > 0 && action != Action::マニピュレーション) {
		state->durability = std::min(craft_status.max_durability, state->durability + 5);
	}
	for (auto& [key, value] : state->buff) {
		value = std::max(0, value - 1);
	}
}

void CrafterCore::ApplyBuffChange(const CraftInfo& craft_status, State* state, const Action& action)
{
	// バフを付けるアクションの処理

	int eff = 0;
	if (state->condition == Condition::長持続) {
		eff = 2;
	}

	switch (action)
	{
	case Action::経過観察:
		state->buff.at(SF::経過観察) = 1;
		break;
	case Action::倹約:
		state->buff.at(SF::倹約) = 4 + eff;
		state->buff.at(SF::長期倹約) = 0;
		break;
	case Action::ヴェネレーション:
		state->buff.at(SF::ヴェネレーション) = 4 + eff;
		break;
	case Action::グレートストライド:
		state->buff.at(SF::グレートストライド) = 3 + eff;
		break;
	case Action::イノベーション:
		state->buff.at(SF::イノベーション) = 4 + eff;
		break;
	case Action::アートオブエレメンタル:
		state->buff.at(SF::アートオブエレメンタル) = 3 + eff;
		break;
	case Action::最終確認:
		state->buff.at(SF::最終確認) = 5 + eff;
		break;
	case Action::長期倹約:
		state->buff.at(SF::倹約) = 0;
		state->buff.at(SF::長期倹約) = 8 + eff;
		break;
	case Action::確信:
		state->buff.at(SF::確信) = 5 + eff;
		break;
	case Action::マニピュレーション:
		state->buff.at(SF::マニピュレーション) = 8 + eff;
		break;
	}
}

void CrafterCore::DeterministicExecuteAction(const CraftInfo& craft_status, State* state, const Action& action, const Condition next_condition, bool is_action_successful)
{
	// ランダム性を排除したアクション処理

	ApplyCPDurabilityChange(craft_status, state, action);
	if (is_action_successful) {
		ApplyProgressChange(craft_status, state, action);
		ApplyQualityChange(craft_status, state, action);
	}
	ApplyInnerQuietChange(craft_status, state, action, is_action_successful);
	ApplyPersistentBuffEffect(craft_status, state, action);
	if (is_action_successful) {
		ApplyBuffChange(craft_status, state, action);
	}
	state->condition = next_condition;
	state->turn += 1;
}

Condition CrafterCore::RandomlyGenNextCondition(const CraftInfo& craft_status, const Condition condition)
{
	// 次の状態をランダムに決定する

	if (condition == Condition::最高品質) {
		return Condition::低品質;
	}

	std::array<float,all_conditions.size()> rates;
	for (int i = 0; i < all_conditions.size(); i++) {
		rates[i] = craft_status.condition_rates.at(all_conditions[i]);
	}

	return all_conditions[rnd.randRates(rates.begin(), rates.end())];
}

std::vector<Action> CrafterCore::AvailableActions(const CraftInfo& craft_status, const State& state)
{
	// 使用可能なアクションを返す

	auto result = std::vector<Action>();
	for (auto& [key, value] : ALL_ACTIONS) {
		if (CanExecuteAction(craft_status, state, key)) {
			result.push_back(key);
		}
	}
	return result;
}

float CrafterCore::SuccessProbability(const State& state,Action action) {
	auto parcentage = ALL_ACTIONS[action].rate;
	if (state.condition == Condition::安定) {
		parcentage += 20;
	}
	if (state.buff.at(SF::経過観察) > 0 && (action == Action::注視作業 || action == Action::注視加工)) {
		parcentage = 100;
	}
	parcentage = std::min(100, parcentage);
	return parcentage < 100 ? rnd.randBool(parcentage / 100.) : 1;
}

std::map<Condition,float> CrafterCore::ConditionProbability(const CraftInfo& craft_status, const State& state) {
	if (state.condition == Condition::最高品質) {
		return std::map<Condition, float> { {Condition::低品質, 1} };
	}

	auto total = 0.f;
	for (const auto& [con, p] : craft_status.condition_rates) {
		total += p;
	}

	auto P = std::map<Condition, float>();
	for (const auto& [con,p] : craft_status.condition_rates) {
		if (p > 0) {
			P.insert({ con, p / total });
		}
	}
	return P;
}

State CrafterCore::ExecuteAction(const CraftInfo& craft_status, State state, Action action)
{
	// アクションを実行する

	if (CanExecuteAction(craft_status, state, action) == false) {
		throw "Can't ExecuteAction";
	}

	auto P = SuccessProbability(state, action);
	auto is_action_successful = P < 1 ? rnd.randBool(P) : true;

	if (action == Action::設計変更) {
		state.設計変更Count += 1;
	}

	auto next_condition = RandomlyGenNextCondition(craft_status, state.condition);

	DeterministicExecuteAction(craft_status, &state, action, next_condition, is_action_successful);
	return state;
}