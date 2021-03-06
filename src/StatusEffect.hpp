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
};

constexpr auto StatusEffectsCount = magic_enum::enum_count<StatusEffect>();