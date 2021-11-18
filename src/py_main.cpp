#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // vector用
#include <pybind11/operators.h>//operator

#include "State.hpp"
#include "StatusEffect.hpp"
#include "CrafterCore.hpp"
#include "CraftInfo.hpp"
#include "Finisher.hpp"

using namespace::std;

namespace py = pybind11;

template <typename E>
auto py_enum(const py::handle& m) {
    constexpr auto enum_name = magic_enum::enum_type_name<E>();
    auto m_enum = py::enum_<E>(m, enum_name.data());
    for (auto val : magic_enum::enum_values<E>()) {
        auto name = magic_enum::enum_name<E>(val);
        m_enum.value(name.data(), val);
    }
    return m_enum;
}

PYBIND11_MODULE(craftcpp, m) {
    m.doc() = "craftcpp";

    py_enum<Condition>(m);

    py_enum<StatusEffect>(m);

    py_enum<Action>(m);

    py::class_<CraftInfo>(m, "CraftInfo")
        .def(py::init<>())
        .def("set", &CraftInfo::set,
            py::arg("max_cp") = py::none(),
            py::arg("max_durability") = py::none(),
            py::arg("max_progress") = py::none(),
            py::arg("max_quality") = py::none(),
            py::arg("base_progress") = py::none(),
            py::arg("iq_table") = py::none(),
            py::arg("illegal_actions") = py::none(),
            py::arg("condition_rates") = py::none()
        )
        .def_readwrite("max_cp", &CraftInfo::max_cp)
        .def_readwrite("max_durability", &CraftInfo::max_durability)
        .def_readwrite("max_progress", &CraftInfo::max_progress)
        .def_readwrite("max_quality", &CraftInfo::max_quality)
        .def_readwrite("base_progress", &CraftInfo::base_progress)
        .def_readwrite("iq_table", &CraftInfo::iq_table)
        .def_readwrite("illegal_actions", &CraftInfo::illegal_actions)
        .def_readwrite("condition_rates", &CraftInfo::condition_rates);
    
    py::class_<State>(m, "State")
        .def(py::init<>())
        .def(py::init<const CraftInfo&>())

        .def(py::pickle(
            [](const State& self) {
                return py::make_tuple(
                    self.cp,
                    self.progress,
                    self.quality,
                    self.durability,
                    self.condition,
                    self.turn,
                    self.buff,
                    self.設計変更Count,
                    self.inner_quiet
                );
            },
            [](py::tuple t){
                if (t.size() != 9)
                    throw std::runtime_error("Invalid state!");
                auto s = State();
                s.cp = t[0].cast<int>();
                s.progress = t[1].cast<int>();
                s.quality = t[2].cast<int>();
                s.durability = t[3].cast<int>();
                s.condition = t[4].cast<Condition>();
                s.turn = t[5].cast<int>();
                s.buff = t[6].cast<std::map<StatusEffect,int>>();
                s.設計変更Count = t[7].cast<int>();
                s.inner_quiet = t[8].cast<int>();
                return s;
            }))

        .def("__str__", &State::to_string)

        .def("get_copy", [](State& self)->auto{return self; }, py::return_value_policy::copy)
        .def("get_score", &State::getScore)
        .def("legal_actions",&State::legal_actions)
        .def("isEnd", &State::isEnd)

        .def_readwrite("cp", &State::cp)
        .def_readwrite("progress", &State::progress)
        .def_readwrite("quality", &State::quality)
        .def_readwrite("durability", &State::durability)
        .def_readwrite("condition", &State::condition)
        .def_readwrite("turn", &State::turn)
        .def_readwrite("buff", &State::buff)
        .def_readwrite("buffs", &State::buff)
        .def_readwrite("設計変更Count", &State::設計変更Count)
        .def_readwrite("inner_quiet", &State::inner_quiet);

    py::class_<ActionParams>(m, "ActionParams")
        .def_readonly("cp", &ActionParams::cp)
        .def_readonly("durability", &ActionParams::durability)
        .def_readonly("rate", &ActionParams::rate)
        .def_readonly("progress", &ActionParams::progress)
        .def_readonly("quality", &ActionParams::quality);

    py::class_<Finisher>(m, "Finisher")
        .def(py::init<const CraftInfo, const std::vector<Action>>())
        .def_readonly("cp",&Finisher::cp)
        .def_readonly("durability", &Finisher::durability)
        .def_readonly("progress", &Finisher::progress)
        .def_readonly("quality", &Finisher::quality)
        .def_readonly("actions", &Finisher::actions);

    py::class_<CrafterCore>(m, "CrafterCore")
        .def_static("_ApplyCPDurabilityChange", &CrafterCore::ApplyCPDurabilityChange)
        .def_static("_ApplyProgressChange", &CrafterCore::ApplyProgressChange)
        .def_static("_ApplyQualityChange", &CrafterCore::ApplyQualityChange)
        .def_static("_ApplyInnerQuietChange", &CrafterCore::ApplyInnerQuietChange)
        .def_static("_ApplyPersistentBuffEffect", &CrafterCore::ApplyPersistentBuffEffect)
        .def_static("_ApplyBuffChange", &CrafterCore::ApplyBuffChange)

        .def_property_readonly_static("ALL_ACTIONS", [](py::object)->auto {return CrafterCore::ALL_ACTIONS.gen_map();})

        .def_static("CanExecuteAction", &CrafterCore::CanExecuteAction)
        .def_static("DeterministicExecuteAction", &CrafterCore::DeterministicExecuteAction)
        .def_static("ExecuteAction",&CrafterCore::ExecuteAction)
        .def_static("AvailableActions", &CrafterCore::AvailableActions)
        .def_static("RandomlyGenNextCondition", &CrafterCore::RandomlyGenNextCondition);
}