import functools
from typing import Callable, List

from .craftcpp import Action, Condition, CrafterCore, CraftInfo, Finisher, State
from .craftcpp import StatusEffect as Buff
from .crafter_base import CrafterBase
from .simcrafter import SimCrafter

ALL_ACTIONS = CrafterCore.ALL_ACTIONS


def one(action: Action):
    def f(s: CrafterBase, _action: Action):
        s.run(_action)
        return

    return functools.partial(f, _action=action)


def many(actions: List[Action], finisher_index=None):
    def f(crafter: CrafterBase, _action: List[Action], finisher_index: int):
        # crafter.state.used_finisher = finisher_index
        for a in _action:
            crafter.run(a)
        return

    return functools.partial(f, _action=actions, finisher_index=finisher_index)


def calc_cp(actions: List[Action]):
    return sum(ALL_ACTIONS[a][0] for a in actions)


def check_cp(state: State, actions: List[Action]):
    return bool(state.cp + sum(ALL_ACTIONS[a].cp for a in actions) >= 0)


def build_finishers(
    info: CraftInfo, finisher_patterns: list[list[Action]]
) -> list[Finisher]:
    FINISHERS = []
    for pattern in finisher_patterns:
        FINISHERS.append(Finisher(info, pattern))
    return FINISHERS


def check_finisher_pattern(state: State, FINISHERS: List[Finisher]):
    def _check_param(durability, cp, quality):
        return (
            state.durability >= durability
            and state.cp >= cp
            and state.quality >= quality
        )

    if not state.inner_quiet == 10:
        return None

    for i, finisher in enumerate(FINISHERS):
        if _check_param(
            durability=finisher.durability, cp=finisher.cp, quality=finisher.quality
        ):
            return many(finisher.actions, i)
        # if finisher.check(state):
        #    return many(finisher.actions,i)
    return None


def should_use_final_appraisal(info: CraftInfo, state: State, action: Action):
    s = state.get_copy()
    if s.buffs[Buff.最終確認] > 0:
        return False
    SimCrafter.DeterministicExecuteAction(info, s, action, Condition.通常, True)
    return s.isEnd(info)


def is_progress_phase(info: CraftInfo, state: State):
    return state.progress + info.base_progress < info.max_progress
