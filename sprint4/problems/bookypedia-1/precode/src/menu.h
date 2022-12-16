#pragma once
#include <functional>
#include <iosfwd>
#include <map>

namespace menu {

class Menu {
public:
    using Handler = std::function<bool(std::istream&)>;

    Menu(std::istream& input, std::ostream& output);

    void AddAction(std::string action_name, std::string args, std::string description,
                   Handler handler);

    void Run();

    void ShowInstructions() const;

private:
    struct ActionInfo {
        Handler handler;
        std::string args;
        std::string description;
    };

    [[nodiscard]] bool ParseCommand(std::istream& input);

    std::istream& input_;
    std::ostream& output_;
    std::map<std::string, ActionInfo> actions_;
};

}  // namespace menu