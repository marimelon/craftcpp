#pragma once
#include <array>

#include "magic_enum.hpp"

enum class Condition : int {
    通常 = 1,
    高品質,
    最高品質,
    低品質,
    安定,
    頑丈,
    高能率,
    高進捗,
    長持続
};

constexpr auto all_conditions = magic_enum::enum_values<Condition>();

/*
const std::array<Condition, 9> all_conditions = {
    Condition::通常,
    Condition::高品質,
    Condition::最高品質,
    Condition::低品質,
    Condition::安定,
    Condition::頑丈,
    Condition::高能率,
    Condition::高進捗,
    Condition::長持続
};
*/