#include "htmldecode.h"
//
#include <iostream>

int main() {
    std::string str;
    std::getline(std::cin, str);
    std::cout << HtmlDecode(str) << std::endl;
}