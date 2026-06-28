#pragma once

#include "magic_enum.hpp"

enum class StatusEffect : int
{
    初手,
    グレートストライド,
    イノベーション,
    マニピュレーション,
    確信,
    倹約,
    長期倹約,
    経過観察,
    最終確認,
    アートオブエレメンタル,
    ヴェネレーション,
    加工,
    中級加工,
    一心不乱,
    匠の好機,	// ヘイスティタッチ成功で付与 → デアリングタッチ解放(1ステップ)
    匠の絶技,	// 次の耐久消費アクションのコストを0にする
};

constexpr auto StatusEffectsCount = magic_enum::enum_count<StatusEffect>();