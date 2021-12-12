from __future__ import annotations

from abc import ABCMeta, abstractmethod

from .craftcpp import Action, CraftInfo, Finisher, State, StatusEffect
from .crafter_base import CrafterBase
from .finisher_pattern import FINISHER_PATTERN


class AutoBase(metaclass=ABCMeta):
    Finishers: list[Finisher] = []

    @classmethod
    @abstractmethod
    def run(
        cls, crafter: CrafterBase, info: CraftInfo, show_progress: bool = False
    ) -> State:
        raise NotImplementedError()

    @classmethod
    def check_should_use_rotation(cls, info: CraftInfo, state: State):
        """妥協フィニッシャーを回す必要があるか"""
        if state.isEnd(info):
            return False
        if state.cp > 74 + 88:
            return False  # 耐久が回復出る場合
        if state.cp <= 74:
            return True  # イノベ+グレスラ+ビエルゴ分のCPが必要
        # 耐久計算
        d = state.durability
        if state.buffs[StatusEffect.マニピュレーション] == 1:
            d += 5
        elif state.buffs[StatusEffect.マニピュレーション] > 1:
            d += 10
        if d < 11:
            return True  # 耐久なし
        return False

    @classmethod
    def setup_finishers(
        cls,
        info: CraftInfo,
        patterns: list[list[Action]] = FINISHER_PATTERN,
        merge=False,
    ):
        if merge == False:
            cls.Finishers = []

        for finisher in patterns:
            try:
                cls.Finishers.append(Finisher(info, finisher))
            except:
                pass
