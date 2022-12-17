#include <iostream>

#include "controller.h"

int main() {
    using namespace std::literals;

    TV tv;
    Menu menu{std::cin, std::cout};
    Controller controller{tv, menu};
    menu.AddAction("Exit"s, {}, "Exits the program"s, [](auto&&...) {
        return false;
    });
    menu.AddAction("Help", {}, "Shows instructions"s, [&menu](auto&&...) {
        menu.ShowInstructions();
        return true;
    });
    menu.ShowInstructions();
    menu.Run();
}
