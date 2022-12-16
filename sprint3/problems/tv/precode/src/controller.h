#pragma once
#include <cassert>

#include "menu.h"
#include "tv.h"

class Controller {
public:
    Controller(TV& tv, Menu& menu)
        : tv_{tv}
        , menu_{menu} {
        using namespace std::literals;
        menu_.AddAction(std::string{INFO_COMMAND}, {}, "Prints info about the TV"s,
                        [this](auto& input, auto& output) {
                            return ShowInfo(input, output);
                        });
        menu_.AddAction(std::string{TURN_ON_COMMAND}, {}, "Turns on the TV"s,
                        [this](auto& input, auto& output) {
                            return TurnOn(input, output);
                        });
        menu_.AddAction(std::string{TURN_OFF_COMMAND}, {}, "Turns off the TV"s,
                        [this](auto& input, auto& output) {
                            return TurnOff(input, output);
                        });
        menu_.AddAction(std::string{SELECT_CHANNEL_COMMAND}, "CHANNEL"s,
                        "Selects the specified channel"s, [this](auto& input, auto& output) {
                            return SelectChannel(input, output);
                        });
        menu_.AddAction(std::string{SELECT_PREVIOUS_CHANNEL_COMMAND}, {},
                        "Selects the previously selected channel"s,
                        [this](auto& input, auto& output) {
                            return SelectPreviousChannel(input, output);
                        });
    }

private:
    /*
     * Обрабатывает команду Info, выводя информацию о состоянии телевизора.
     * Если телевизор выключен, выводит "TV is turned off"
     * Если телевизор включен, выводит две строки:
     * TV is turned on
     * Channel number is <номер канала>
     * Если в input содержатся какие-либо параметры, выводит сообщение об ошибке:
     * Error: the Info command does not require any arguments
     */
    [[nodiscard]] bool ShowInfo(std::istream& input, std::ostream& output) const {
        using namespace std::literals;

        if (EnsureNoArgsInInput(INFO_COMMAND, input, output)) {
            if (tv_.IsTurnedOn()) {
                // Эта часть метода не реализована. Реализуйте её самостоятельно
                assert(!"Controller::ShowInfo is not implemented when TV is turned on");
                /*
                Выведите две строки, завершая каждую std::endl:

                TV is turned on
                Channel number is <номер канала>
                */
            } else {
                output << "TV is turned off"sv << std::endl;
            }
        }

        return true;
    }

    /*
     * Обрабатывает команду TurnOn, включая телевизор
     * Если в input содержатся какие-либо параметры, не включает телевизор и выводит сообщение
     * об ошибке:
     * Error: the TurnOff command does not require any arguments
     */
    [[nodiscard]] bool TurnOn(std::istream& input, std::ostream& output) const {
        using namespace std::literals;

        if (EnsureNoArgsInInput(TURN_ON_COMMAND, input, output)) {
            tv_.TurnOn();
        }
        return true;
    }

    /*
     * Обрабатывает команду TurnOff, выключая телевизор
     * Если в input содержатся какие-либо параметры, не выключает телевизор и выводит сообщение
     * об ошибке:
     * Error: the TurnOff command does not require any arguments
     */
    [[nodiscard]] bool TurnOff(std::istream& input, std::ostream& output) const {
        using namespace std::literals;

        if (EnsureNoArgsInInput(TURN_OFF_COMMAND, input, output)) {
            tv_.TurnOff();
        }
        return true;
    }

    /*
     * Обрабатывает команду SelectChannel <номер канала>
     * Выбирает заданный номер канала на tv_.
     * Если номер канала - не целое число, выводит в output ошибку "Invalid channel"
     * Обрабатывает ошибки переключения каналов на телевизор и выводит в output сообщения:
     * - "Channel is out of range", если TV::SelectChannel выбросил std::out_of_range
     * - "TV is turned off", если TV::SelectChannel выбросил std::logic_error
     */
    [[nodiscard]] bool SelectChannel(std::istream& input, std::ostream& output) const {
        /* Реализуйте самостоятельно этот метод.*/
        assert(!"TODO: Implement Controller::SelectChannel");
        return true;
    }

    /*
     * Обрабатывает команду SelectPreviousChannel, выбирая предыдущий канал на tv_.
     * Если TV::SelectLastViewedChannel выбросил std::logic_error, выводит в output сообщение:
     * "TV is turned off"
     */
    [[nodiscard]] bool SelectPreviousChannel(std::istream& input, std::ostream& output) const {
        /* Реализуйте самостоятельно этот метод */
        assert(!"TODO: Implement Controller::SelectPreviousChannel");
        return true;
    }

    [[nodiscard]] bool EnsureNoArgsInInput(std::string_view command, std::istream& input,
                                           std::ostream& output) const {
        using namespace std::literals;
        assert(input);
        if (std::string data; input >> data) {
            output << "Error: the " << command << " command does not require any arguments"sv
                   << std::endl;
            return false;
        }
        return true;
    }

    constexpr static std::string_view INFO_COMMAND = "Info";
    constexpr static std::string_view TURN_ON_COMMAND = "TurnOn";
    constexpr static std::string_view TURN_OFF_COMMAND = "TurnOff";
    constexpr static std::string_view SELECT_CHANNEL_COMMAND = "SelectChannel";
    constexpr static std::string_view SELECT_PREVIOUS_CHANNEL_COMMAND = "SelectPreviousChannel";

    TV& tv_;
    Menu& menu_;
};
