from __future__ import annotations

from random import random

from .craftcpp import Action, Condition, CrafterCore, CraftInfo, State
from .craftcpp import StatusEffect as Buff
from .crafter_base import CrafterBase

ALL_ACTIONS = CrafterCore.ALL_ACTIONS


class CantUseActionException(Exception):
    ...


class SimCrafter(CrafterBase):
    def __init__(self, info: CraftInfo, state: State = None):
        self.info = info
        if state is None:
            self._state = State(info)
        else:
            self._state = state

    @staticmethod
    def CanExecuteAction(info: CraftInfo, state: State, action: Action) -> bool:
        """アクションが実行可能かどうか判定する"""
        return CrafterCore.CanExecuteAction(info, state, action)

    @staticmethod
    def ApplyCPDurabilityChange(info: CraftInfo, state: State, action: Action):
        """CP,耐久度の増減を適応する"""
        CrafterCore._ApplyCPDurabilityChange(info, state, action)

    @staticmethod
    def ApplyProgressChange(info: CraftInfo, state: State, action: Action):
        CrafterCore._ApplyProgressChange(info, state, action)

    @staticmethod
    def ApplyQualityChange(info: CraftInfo, state: State, action: Action):
        CrafterCore._ApplyQualityChange(info, state, action)

    @staticmethod
    def ApplyInnerQuietChange(
        info: CraftInfo, state: State, action: Action, is_action_successful: bool
    ):
        CrafterCore._ApplyInnerQuietChange(info, state, action, is_action_successful)

    @staticmethod
    def ApplyPersistentBuffEffect(info: CraftInfo, state: State, action: Action):
        CrafterCore._ApplyPersistentBuffEffect(info, state, action)

    @staticmethod
    def ApplyBuffChange(info: CraftInfo, state: State, action: Action):
        CrafterCore._ApplyBuffChange(info, state, action)

    @staticmethod
    def DeterministicExecuteAction(
        info: CraftInfo,
        state: State,
        action: Action,
        next_condition: Condition,
        is_action_successful: bool,
        manual: bool = False,
    ):
        if manual:
            SimCrafter.ApplyCPDurabilityChange(info, state, action)
            if is_action_successful:
                SimCrafter.ApplyProgressChange(info, state, action)
                SimCrafter.ApplyQualityChange(info, state, action)
            SimCrafter.ApplyInnerQuietChange(info, state, action, is_action_successful)
            SimCrafter.ApplyPersistentBuffEffect(info, state, action)
            if is_action_successful:
                SimCrafter.ApplyBuffChange(info, state, action)
            state.condition = next_condition
            state.turn += 1
        else:
            CrafterCore.DeterministicExecuteAction(
                info, state, action, next_condition, is_action_successful
            )

    @staticmethod
    def RandomlyGenNextCondition(info: CraftInfo, conditon: Condition):
        return CrafterCore.RandomlyGenNextCondition(info, conditon)

    @staticmethod
    def ExecuteAction(
        info: CraftInfo, state: State, action: Action, manual: bool = False
    ):
        try:
            if manual:
                if not SimCrafter.CanExecuteAction(info, state, action):
                    raise Exception("Can't ExecuteAction")
                parcentage: float = ALL_ACTIONS[action].rate
                if state.condition == Condition.安定:
                    parcentage += 20

                if state.buff[Buff.経過観察] > 0 and (
                    action == Action.注視作業 or action == Action.注視加工
                ):
                    parcentage = 100
                parcentage = min(100, parcentage)

                is_action_successful = (
                    random() < (parcentage / 100.0) if parcentage < 100 else True
                )

                if action == Action.設計変更:
                    state.設計変更Count += 1

                next_condition = SimCrafter.RandomlyGenNextCondition(
                    info, state.condition
                )
                SimCrafter.DeterministicExecuteAction(
                    info, state, action, next_condition, is_action_successful, manual
                )
                return state

            else:
                return CrafterCore.ExecuteAction(info, state, action)
        except Exception as e:
            print(info, state, action)
            raise e

    def run(
        self,
        action: Action,
        state: State = None,
    ):
        if state is None:
            state = self._state

        if action == Action.倹約加工 and (
            state.buffs[Buff.倹約] > 0 or state.buffs[Buff.長期倹約] > 0
        ):
            action = Action.加工

        self._state = self.ExecuteAction(self.info, state, action)

        return self._state

    def get_state_copy(self):
        return self._state.get_copy()

    @property
    def state(self) -> State:
        return self._state
