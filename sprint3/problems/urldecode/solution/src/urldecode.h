#pragma once

#include <string>

/*
Возвращает URL-декодированное представление строки str.
Пример: "Hello+World%20%21" должна превратиться в "Hello World !"
В случае ошибки выбрасывает исключение std::invalid_argument
*/
std::string UrlDecode(std::string_view str);
