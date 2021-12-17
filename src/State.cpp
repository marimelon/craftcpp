#include "State.hpp"
#include "CrafterCore.hpp"

std::string State::to_string()
{
    std::string str = "";
    str += "CP= " + std::to_string(this->cp);
    str += ", DU= " + std::to_string(this->durability);
    str += ", PR= " + std::to_string(this->progress);
    str += ", QU= " + std::to_string(this->quality);
    str += ", CO= " + std::string(magic_enum::enum_name(this->condition));
    str += ", TU= " + std::to_string(this->turn);
    str += ", IQ= " + std::to_string(this->inner_quiet);
    str += ", OP= " + std::to_string(this->設計変更Count) + "|" + std::to_string(this->一心不乱Count);
    str += ", BU= {";
    for (const auto &[k, v] : this->buff)
    {
        if (v > 0)
        {
            str += std::string(magic_enum::enum_name(k));
            str += ":" + std::to_string(v);
            str += ",";
        }
    }
    str += "}";
    return str;
}

std::set<Action> State::legal_actions(const CraftInfo &info)
{
    auto result = std::set<Action>();
    for (const auto &[action, param] : CrafterCore::ALL_ACTIONS)
    {
        if (std::find(info.illegal_actions.begin(), info.illegal_actions.end(), action) != info.illegal_actions.end())
        {
            continue;
        }
        if (!CrafterCore::CanExecuteAction(info, *this, action))
        {
            continue;
        }
        result.insert(action);
    }
    return result;
}
