#include <iostream>

#include "urldecode.h"

int main() {
    using namespace std::literals;

    try {
        std::string s;
        std::getline(std::cin, s);

        std::cout << UrlDecode(s) << std::endl;

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Error: "sv << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error"sv << std::endl;
    }

    return EXIT_FAILURE;
}
