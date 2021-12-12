import time
import re

from .net.CraftRunner import CraftRunner
from .net.ChatReader import ChatReader

from .crafter_base import CrafterBase
from .craftcpp import Action, State, Condition
from .craftcpp import StatusEffect as Buff


class InGameCrafter(CrafterBase):
    default_設計変更Count = 0

    def __init__(self, pid: int = None):
        super().__init__()
        self.craft_runner = CraftRunner(pid) if pid else CraftRunner()
        self.設計変更Count = InGameCrafter.default_設計変更Count
        self.playerName = self.craft_runner.Read().player_name
        self.FINISH_REGEX = re.compile(self.playerName + r"は(?P<itemname>.*?)を完成させた！")
        self.FAILD_REGEX = re.compile(self.playerName + r"は製作に失敗した……")
        self.history = []

    def reset(self):
        self.設計変更Count = InGameCrafter.default_設計変更Count
        self.history = []
        return State()

    def set_now_state(self):
        craftState = self.craft_runner.Read()
        self.progress = craftState.progress
        self.quality = craftState.quality
        self.durability = craftState.durability
        self.condition = craftState.condition
        self.turn = craftState.turn
        self.cp = craftState.cp

        self.inner_quiet = 0
        self.buffs = {b: 0 for b in Buff.values()}

        if craftState.previous_action == "経過観察":
            self.buffs[Buff.経過観察] = 1
        if craftState.previous_action == "加工":
            self.buffs[Buff.加工] = 1

        for buff in craftState.buffs:
            if buff.name == "インナークワイエット":
                self.inner_quiet = buff.stack
                continue

            for b in Buff.values():
                if buff.name == b.name:
                    self.buffs[b] = buff.stack
                    break

    def get_state_copy(self):
        craftState = self.craft_runner.Read()

        state = State()
        state.cp = craftState.cp
        state.progress = craftState.progress
        state.quality = craftState.quality
        state.durability = craftState.durability
        state.condition = Condition(craftState.condition)
        state.turn = craftState.turn

        if craftState.previous_action == "経過観察":
            state.buffs[Buff.経過観察] = 1
        if craftState.previous_action == "加工":
            state.buffs[Buff.加工] = 1

        for buff in craftState.buffs:
            if buff.name == "インナークワイエット":
                state.inner_quiet = buff.stack
                continue
            for b in Buff.values():
                if buff.name == b.name:
                    state.buffs[b] = buff.stack
                    break
        state.設計変更Count = self.設計変更Count
        return state

    def run(self, action: Action, wait=False):
        current = self.get_state_copy()
        if action == Action.倹約加工:
            if current.buffs[Buff.長期倹約] > 0:
                action = Action.加工

        self.craft_runner.RunAction(action.name)
        if action == Action.設計変更:
            self.設計変更Count += 1
            time.sleep(0.5)

        chat_reader = ChatReader()
        while self.craft_runner.CanAction("作業") == 579:
            if any(
                self.FINISH_REGEX.match(item.Line) or self.FAILD_REGEX.match(item.Line)
                for item in chat_reader.ReadChatLog()
            ):
                break
            time.sleep(0.2)

        # input()
        # time.sleep(0.5)

        self.history.append(action)

        self.set_now_state()

    @property
    def state(self) -> State:
        return self.get_state_copy()


if __name__ == "__main__":
    crafter = InGameCrafter()
    state = crafter.get_state_copy()
    print(state)
    # crafter.craft_runner.RunAction("作業")
