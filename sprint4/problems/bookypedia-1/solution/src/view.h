#pragma once
#include <iosfwd>

namespace menu {
class Menu;
}

namespace app {
class UseCases;
}

namespace ui {

class View {
public:
    View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output);

private:
    bool AddAuthor(std::istream& cmd_input);
    bool ShowAuthors();
    bool AddBook(std::istream& cmd_input);
    bool ShowBooks();

    menu::Menu& menu_;
    app::UseCases& use_cases_;
    std::istream& input_;
    std::ostream& output_;
};

}  // namespace ui