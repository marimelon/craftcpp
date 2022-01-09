import craftpy as m
from craftpy import craftcpp


def get_default_env():
    info = m.craftcpp.CraftInfo()
    info.base_progress = 100
    info.base_quality = 100
    info.max_progress = 10000
    info.max_quality = 10000
    info.max_durability = 100
    info.max_cp = 1000
    return info


def test_main():
    # CraftCoreの動作テスト

    env = get_default_env()
    s = m.craftcpp.State(env)
    m.craftcpp.CrafterCore.CanExecuteAction(env, s, m.craftcpp.Action.作業)
    m.craftcpp.CrafterCore.ExecuteAction(env, s, m.craftcpp.Action.作業)


def test_normal_actions():
    # Progress,Quality,Cp,Durabilityのテスト
    env = get_default_env()

    # CP,PR,QU,DU
    corrects = {
        m.craftcpp.Action.作業: (0, 120, 0, -10),
        m.craftcpp.Action.加工: (-18, 0, 100, -10),
        m.craftcpp.Action.マスターズメンド: (-88, 0, 0, 0),
        m.craftcpp.Action.ヘイスティタッチ: (0, 0, 100, -10),
        m.craftcpp.Action.突貫作業: (0, 500, 0, -10),
        m.craftcpp.Action.経過観察: (-7, 0, 0, 0),
        # m.craftcpp.Action.秘訣: (20,0, 0, 0),
        m.craftcpp.Action.倹約: (-56, 0, 0, 0),
        m.craftcpp.Action.ヴェネレーション: (-18, 0, 0, 0),
        m.craftcpp.Action.中級加工: (-32, 0, 125, -10),
        m.craftcpp.Action.グレートストライド: (-32, 0, 0, 0),
        m.craftcpp.Action.イノベーション: (-18, 0, 0, 0),
        m.craftcpp.Action.最終確認: (-1, 0, 0, 0),
        m.craftcpp.Action.長期倹約: (-98, 0, 0, 0),
        # m.craftcpp.Action.ビエルゴの祝福: (-24,0, 0, 0),
        # m.craftcpp.Action.集中加工: (-18,0, 0, 0),
        m.craftcpp.Action.確信: (-6, 300, 0, -10),
        m.craftcpp.Action.模範作業: (-7, 180, 0, -10),
        m.craftcpp.Action.マニピュレーション: (-96, 0, 0, 0),
        m.craftcpp.Action.倹約加工: (-25, 0, 100, -5),
        m.craftcpp.Action.注視作業: (-5, 200, 0, -10),
        m.craftcpp.Action.注視加工: (-18, 0, 150, -10),
        m.craftcpp.Action.真価: (-6, 0, 100, -10),
        m.craftcpp.Action.下地加工: (-40, 0, 200, -20),
        m.craftcpp.Action.下地作業: (-18, 360, 0, -20),
        m.craftcpp.Action.精密作業: (-32, 100, 100, -10),
        # m.craftcpp.Action.集中作業: (-6,0, 0, 0),
        m.craftcpp.Action.匠の早業: (-250, 0, 0, 0),
        m.craftcpp.Action.上級加工: (-46, 0, 150, -10),
        m.craftcpp.Action.倹約作業: (-18, 180, 0, -5),
        # m.craftcpp.Action.匠の神業: (-32,0, 0, 0),
        m.craftcpp.Action.設計変更: (0, 0, 0, 0),
        m.craftcpp.Action.一心不乱: (0, 0, 0, 0),
    }

    for action, correct in corrects.items():
        s = m.craftcpp.State(env)
        assert m.craftcpp.CrafterCore.CanExecuteAction(env, s, action), f"state = {s}"
        m.craftcpp.CrafterCore.DeterministicExecuteAction(
            env, s, action, m.craftcpp.Condition.通常, True
        )
        assert s.cp == env.max_cp + correct[0], f"error cp : state = {s}"
        assert s.progress == correct[1], f"error progress : state = {s}"
        assert s.quality == correct[2], f"error quality : state = {s}"
        assert (
            s.durability == env.max_durability + correct[3]
        ), f"error durability : state = {s}"


def test_status_effect_turn():
    # バフの付与ターンのテスト

    env = get_default_env()

    corrects = {
        m.craftcpp.Action.経過観察: (m.craftcpp.StatusEffect.経過観察, 1),
        m.craftcpp.Action.倹約: (m.craftcpp.StatusEffect.倹約, 4),
        m.craftcpp.Action.ヴェネレーション: (m.craftcpp.StatusEffect.ヴェネレーション, 4),
        m.craftcpp.Action.グレートストライド: (m.craftcpp.StatusEffect.グレートストライド, 3),
        m.craftcpp.Action.イノベーション: (m.craftcpp.StatusEffect.イノベーション, 4),
        m.craftcpp.Action.最終確認: (m.craftcpp.StatusEffect.最終確認, 5),
        m.craftcpp.Action.長期倹約: (m.craftcpp.StatusEffect.長期倹約, 8),
        m.craftcpp.Action.確信: (m.craftcpp.StatusEffect.確信, 5),
        m.craftcpp.Action.マニピュレーション: (m.craftcpp.StatusEffect.マニピュレーション, 8),
        m.craftcpp.Action.一心不乱: (m.craftcpp.StatusEffect.一心不乱, 1),
    }

    for action, correct in corrects.items():
        s = m.craftcpp.State(env)
        m.craftcpp.CrafterCore.DeterministicExecuteAction(
            env, s, action, m.craftcpp.Condition.通常, True
        )
        assert s.buffs[correct[0]] == correct[1]


def test_status_effect_effect():
    # バフ効果のテスト

    env = get_default_env()
    core = m.craftcpp.CrafterCore
    ac = m.craftcpp.Action
    sf = m.craftcpp.StatusEffect

    # 倹約
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.倹約)
    s = core.ExecuteAction(env, s, ac.作業)
    assert s.durability == env.max_durability - 5
    assert s.buff[sf.倹約] == 3

    # ヴェネレーション
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.ヴェネレーション)
    s = core.ExecuteAction(env, s, ac.作業)
    assert s.progress == 180  # 作業(+50%)
    assert s.buff[sf.ヴェネレーション] == 3

    # グレートストライド
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.グレートストライド)
    s = core.ExecuteAction(env, s, ac.加工)
    assert s.quality == 200
    assert s.buff[sf.グレートストライド] == 0

    # イノベーション
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.イノベーション)
    s = core.ExecuteAction(env, s, ac.加工)
    assert s.quality == 150
    assert s.buff[sf.イノベーション] == 3

    # 最終確認
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.最終確認)
    s.progress = env.max_progress - 10
    s = core.ExecuteAction(env, s, ac.作業)
    assert s.progress == env.max_progress - 1
    assert s.buff[sf.最終確認] == 0

    # 長期倹約
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.長期倹約)
    s = core.ExecuteAction(env, s, ac.作業)
    assert s.durability == env.max_durability - 5
    assert s.buff[sf.長期倹約] == 7

    # 確信
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.確信)
    s = core.ExecuteAction(env, s, ac.作業)
    assert s.progress == 300 + 120 * 2  # 確信 + 作業(+100%)
    assert s.buff[sf.確信] == 0
    assert core.CanExecuteAction(env, s, ac.確信) == False

    # マニピュレーション
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.マニピュレーション)
    s = core.ExecuteAction(env, s, ac.作業)
    assert s.durability == env.max_durability - 5
    s = core.ExecuteAction(env, s, ac.作業)
    assert s.durability == env.max_durability - 10
    s = core.ExecuteAction(env, s, ac.経過観察)
    assert s.durability == env.max_durability - 5

    # 一心不乱
    s = m.craftcpp.State(env)
    core.DeterministicExecuteAction(env, s, ac.一心不乱, m.craftcpp.Condition.通常, True)
    assert (
        s.condition == m.craftcpp.Condition.通常
        and m.craftcpp.CrafterCore.CanExecuteAction(env, s, ac.秘訣) == True
    )

    # 倹約 & 長期倹約
    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.倹約)
    s = core.ExecuteAction(env, s, ac.長期倹約)
    assert s.buff[sf.倹約] == 0
    assert s.buff[sf.長期倹約] == 8

    s = m.craftcpp.State(env)
    s = core.ExecuteAction(env, s, ac.長期倹約)
    s = core.ExecuteAction(env, s, ac.倹約)
    assert s.buff[sf.長期倹約] == 0
    assert s.buff[sf.倹約] == 4


def test_condition_can_action():
    # 高品質時のみ利用可能なアクションのテスト

    env = get_default_env()

    actions = [
        m.craftcpp.Action.秘訣,
        m.craftcpp.Action.ビエルゴの祝福,
        m.craftcpp.Action.集中加工,
        m.craftcpp.Action.集中作業,
    ]

    for action in actions:
        s = m.craftcpp.State(env)
        assert m, craftcpp.CrafterCore.CanExecuteAction(env, s, action) == False
        s.condition = m.craftcpp.Condition.高品質
        assert m, craftcpp.CrafterCore.CanExecuteAction(env, s, action) == True


def test_equal():
    # 等価演算子のテスト

    # CraftInfo
    info1 = m.craftcpp.CraftInfo()
    info2 = m.craftcpp.CraftInfo()
    assert info1 == info2
    info1.max_cp += 1
    assert info1 != info2

    # State
    s1 = m.craftcpp.State()
    s2 = m.craftcpp.State()
    assert s1 == s2
    s1.cp += 1
    assert s1 != s2

def test_pickles():
    # pickle化のテスト
    import pickle

    # CraftInfo
    info = m.craftcpp.CraftInfo()
    pickled = pickle.dumps(info)
    unpickled = pickle.loads(pickled)
    assert info == unpickled

    # State
    state = m.craftcpp.State()
    pickled = pickle.dumps(state)
    unpickled = pickle.loads(pickled)
    assert state == unpickled
