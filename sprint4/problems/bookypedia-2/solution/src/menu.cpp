#include "menu.h"

#include <iomanip>
#include <sstream>

namespace menu {

Menu::Menu(std::istream& input, std::ostream& output)
    : input_{input}
    , output_{output} {
}

void Menu::AddAction(std::string action_name, std::string args, std::string description,
                     Handler handler) {
    if (!actions_
             .try_emplace(std::move(action_name), std::move(handler), std::move(args),
                          std::move(description))
             .second) {
        throw std::invalid_argument("A command has been added already");
    }
}

void Menu::Run() {
    std::string line;
    while (std::getline(input_, line)) {
        std::istringstream cmd_stream{std::move(line)};
        if (!ParseCommand(cmd_stream)) {
            break;
        }
    }
}

void Menu::ShowInstructions() const {
    if (actions_.empty()) {
        return;
    }
    size_t actions_width = 0;
    size_t args_width = 0;
    for (const auto& [action_name, info] : actions_) {
        actions_width = std::max(actions_width, action_name.length());
        args_width = std::max(args_width, info.args.length());
    }

    const auto old_flags = output_.flags();
    const auto old_fill = output_.fill();
    auto restore_flags = [this, old_fill, old_flags] {
        output_.fill(old_fill);
        output_.setf(old_flags);
    };

    try {
        output_ << std::left << std::setfill(' ');
        for (const auto& [action_name, info] : actions_) {
            output_ << std::setw(actions_width + 1) << action_name;
            output_ << std::setw(args_width + 1) << info.args;
            output_ << info.description << std::endl;
        }
    } catch (...) {
        restore_flags();
        throw;
    }
    restore_flags();
}

bool Menu::ParseCommand(std::istream& input) {
    using namespace std::literals;

    try {
        std::string cmd;
        if (input >> cmd) {
            if (const auto it = actions_.find(cmd); it != actions_.cend()) {
                if (!it->second.handler(input)) {
                    return false;
                }
            } else {
                output_ << "Command '"sv << cmd << "' has not been found."sv << std::endl;
            }
        } else {
            output_ << "Invalid command"sv << std::endl;
        }
    } catch (const std::exception& e) {
        output_ << e.what() << std::endl;
    }
    return true;
}

}  // namespace menu
