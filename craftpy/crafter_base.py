from __future__ import annotations

from abc import ABCMeta, abstractmethod

from .craftcpp import Action, State


class CrafterBase(metaclass=ABCMeta):
    @property
    @abstractmethod
    def state(self) -> State:
        ...

    @abstractmethod
    def get_state_copy(self) -> State:
        raise NotImplementedError()

    @abstractmethod
    def run(self, action: Action, state: State = None) -> State:
        raise NotImplementedError()
