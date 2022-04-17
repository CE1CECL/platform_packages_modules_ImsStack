#include "call/state/MtcCallStateMachine.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

class TestState
{
public:
    enum class StateName
    {
        FIRST = '1',
        SECOND = '2',
    };

    TestState(StateName eState, std::string& strLog) :
            m_eState(eState),
            m_strLog(strLog) {}

    void OnEnter()
    {
        m_strLog.append(1, static_cast<char>(m_eState));
        m_strLog.append("enter ");
    }

    void OnExit()
    {
        m_strLog.append(1, static_cast<char>(m_eState));
        m_strLog.append("exit ");
    }

    StateName GetStateName() const { return m_eState; }

    StateName m_eState;
    std::string& m_strLog;
};

class TestStateFactory :
        public IMtcCallStateFactory<TestState, TestState::StateName>
{
public:
    TestState* CreateState(TestState::StateName eState) override
    {
        return new TestState(eState, m_strLog);
    }

    std::string m_strLog;
};

class TestStateWatcher :
        public IMtcCallStateWatcher<TestState::StateName>
{
public:
    void OnStateTransition(IN TestState::StateName eState) override
    {
        m_strLog.append(1, static_cast<char>(eState));
    }

    std::string m_strLog;
};

TEST(MtcCallStateMachineTest, GetStateReturnsCurrentStateName)
{
    TestStateFactory objFactory;
    MtcCallStateMachine<TestState, TestState::StateName> objStateMachine(
            TestState::StateName::FIRST, objFactory);

    EXPECT_EQ(TestState::StateName::FIRST, objStateMachine.GetState());
}

TEST(MtcCallStateMachineTest, InitialStateCallsOnEnter)
{
    TestStateFactory objFactory;
    MtcCallStateMachine<TestState, TestState::StateName> objStateMachine(
            TestState::StateName::FIRST, objFactory);

    EXPECT_STREQ("1enter ", objFactory.m_strLog.c_str());
}

TEST(MtcCallStateMachineTest, InitialStateNotifiesWatcher)
{
    TestStateFactory objFactory;
    TestStateWatcher objWatcher;
    MtcCallStateMachine<TestState, TestState::StateName> objStateMachine(
            TestState::StateName::FIRST, objFactory, &objWatcher);

    EXPECT_STREQ("1", objWatcher.m_strLog.c_str());
}

TEST(MtcCallStateMachineTest, TransitionToSameStateCallsOnEnter)
{
    TestStateFactory objFactory;
    MtcCallStateMachine<TestState, TestState::StateName> objStateMachine(
            TestState::StateName::FIRST, objFactory);

    objStateMachine.RunStateOperation([](TestState* /* pState */)
    {
        return TestState::StateName::FIRST;
    });

    EXPECT_STREQ("1enter ", objFactory.m_strLog.c_str());
}

TEST(MtcCallStateMachineTest, TransitionToSameStateNotNotifiesWatcher)
{
    TestStateFactory objFactory;
    TestStateWatcher objWatcher;
    MtcCallStateMachine<TestState, TestState::StateName> objStateMachine(
            TestState::StateName::FIRST, objFactory, &objWatcher);

    objStateMachine.RunStateOperation([](TestState* /* pState */)
    {
        return TestState::StateName::FIRST;
    });

    EXPECT_STREQ("1", objWatcher.m_strLog.c_str());
}

TEST(MtcCallStateMachineTest, TransitionToAnotherStateCallsOnExitAndOnEnter)
{
    TestStateFactory objFactory;
    MtcCallStateMachine<TestState, TestState::StateName> objStateMachine(
            TestState::StateName::FIRST, objFactory);

    objStateMachine.RunStateOperation([](TestState* /* pState */)
    {
        return TestState::StateName::SECOND;
    });

    EXPECT_STREQ("1enter 1exit 2enter ", objFactory.m_strLog.c_str());
}

TEST(MtcCallStateMachineTest, TransitionToAnotherStateNotifiesWatcher)
{
    TestStateFactory objFactory;
    TestStateWatcher objWatcher;
    MtcCallStateMachine<TestState, TestState::StateName> objStateMachine(
            TestState::StateName::FIRST, objFactory, &objWatcher);

    objStateMachine.RunStateOperation([](TestState* /* pState */)
    {
        return TestState::StateName::SECOND;
    });

    EXPECT_STREQ("12", objWatcher.m_strLog.c_str());
}
