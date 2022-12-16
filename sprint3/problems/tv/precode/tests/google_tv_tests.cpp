#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "../src/controller.h"

// Тестовый стенд "Телевизор по умолчанию"
class TVByDefault : public testing::Test {
protected:
    TV tv_;
};
TEST_F(TVByDefault, IsOff) {
    EXPECT_FALSE(tv_.IsTurnedOn());
}
TEST_F(TVByDefault, DoesntShowAChannelWhenItIsOff) {
    EXPECT_FALSE(tv_.GetChannel().has_value());
}
// Включите этот тест и доработайте класс TV, чтобы тест выполнился успешно
#if 0
TEST_F(TVByDefault, CantSelectAnyChannel) {
    EXPECT_THROW(tv_.SelectChannel(10), std::logic_error);
    EXPECT_EQ(tv_.GetChannel(), std::nullopt);
    tv_.TurnOn();
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(1));
}
#endif

// Тестовый стенд "Включенный телевизор"
class TurnedOnTV : public TVByDefault {
protected:
    void SetUp() override {
        tv_.TurnOn();
    }
};
TEST_F(TurnedOnTV, ShowsChannel1) {
    EXPECT_TRUE(tv_.IsTurnedOn());
    EXPECT_THAT(tv_.GetChannel(), testing::Optional(1));
}
TEST_F(TurnedOnTV, AfterTurningOffTurnsOffAndDoesntShowAnyChannel) {
    tv_.TurnOff();
    EXPECT_FALSE(tv_.IsTurnedOn());
    // Сравнение с nullopt в GoogleTest выполняется так:
    EXPECT_EQ(tv_.GetChannel(), std::nullopt);
}
TEST_F(TurnedOnTV, CanSelectChannelFrom1To99) {
    /* Реализуйте самостоятельно этот тест */
}
/* Реализуйте самостоятельно остальные тесты класса TV */

//-----------------------------------------------------------------------------------

using namespace std::literals;

class ControllerWithTurnedOffTV : public testing::Test {
protected:
    void SetUp() override {
        ASSERT_FALSE(tv_.IsTurnedOn());
    }

    void RunMenuCommand(std::string command) {
        input_.str(std::move(command));
        input_.clear();
        menu_.Run();
    }

    void ExpectExtraArgumentsErrorInOutput(std::string_view command) const {
        ExpectOutput(
            "Error: the "s.append(command).append(" command does not require any arguments\n"sv));
    }

    void ExpectEmptyOutput() const {
        ExpectOutput({});
    }

    void ExpectOutput(std::string_view expected) const {
        // В g++ 10.3 не реализован метод ostringstream::view(), поэтому приходится
        // использовать метод str()
        EXPECT_EQ(output_.str(), std::string{expected});
    }

    TV tv_;
    std::istringstream input_;
    std::ostringstream output_;
    Menu menu_{input_, output_};
    Controller controller_{tv_, menu_};
};

TEST_F(ControllerWithTurnedOffTV, OnInfoCommandPrintsThatTVIsOff) {
    input_.str("Info"s);
    menu_.Run();
    ExpectOutput("TV is turned off\n"sv);
    EXPECT_FALSE(tv_.IsTurnedOn());
}
TEST_F(ControllerWithTurnedOffTV, OnInfoCommandPrintsErrorMessageIfCommandHasAnyArgs) {
    RunMenuCommand("Info some extra args"s);
    EXPECT_FALSE(tv_.IsTurnedOn());
    ExpectExtraArgumentsErrorInOutput("Info"sv);
}
TEST_F(ControllerWithTurnedOffTV, OnInfoCommandWithTrailingSpacesPrintsThatTVIsOff) {
    input_.str("Info  "s);
    menu_.Run();
    ExpectOutput("TV is turned off\n"sv);
}
TEST_F(ControllerWithTurnedOffTV, OnTurnOnCommandTurnsTVOn) {
    RunMenuCommand("TurnOn"s);
    EXPECT_TRUE(tv_.IsTurnedOn());
    ExpectEmptyOutput();
}
TEST_F(ControllerWithTurnedOffTV, OnTurnOnCommandPrintsErrorMessageIfCommandHasAnyArgs) {
    RunMenuCommand("TurnOn some extra args"s);
    EXPECT_FALSE(tv_.IsTurnedOn());
    ExpectExtraArgumentsErrorInOutput("TurnOn"sv);
}
/*
 * Протестируйте остальные аспекты поведения класса Controller, когда TV выключен
 */

//-----------------------------------------------------------------------------------

class ControllerWithTurnedOnTV : public ControllerWithTurnedOffTV {
protected:
    void SetUp() override {
        tv_.TurnOn();
    }
};

TEST_F(ControllerWithTurnedOnTV, OnTurnOffCommandTurnsTVOff) {
    RunMenuCommand("TurnOff"s);
    EXPECT_FALSE(tv_.IsTurnedOn());
    ExpectEmptyOutput();
}
TEST_F(ControllerWithTurnedOnTV, OnTurnOffCommandPrintsErrorMessageIfCommandHasAnyArgs) {
    RunMenuCommand("TurnOff some extra args"s);
    EXPECT_TRUE(tv_.IsTurnedOn());
    ExpectExtraArgumentsErrorInOutput("TurnOff"sv);
}
// Включите этот тест, после того, как реализуете метод TV::SelectChannel
#if 0
TEST_F(ControllerWithTurnedOnTV, OnInfoPrintsCurrentChannel) {
    tv_.SelectChannel(42);
    RunMenuCommand("Info"s);
    ExpectOutput("TV is turned on\nChannel number is 42\n"sv);
}
#endif
/*
 * Протестируйте остальные аспекты поведения класса Controller, когда TV включен
 */
