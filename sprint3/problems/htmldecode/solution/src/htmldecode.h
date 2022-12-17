#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

/*
 * Декодирует основные HTML-мнемоники:
 * - &lt - <
 * - &gt - >
 * - &amp - &
 * - &pos - '
 * - &quot - "
 *
 * Мнемоника может быть записана целиком либо строчными, либо заглавными буквами:
 * - &lt и &LT декодируются как <
 * - &Lt и &lT не мнемоники
 *
 * После мнемоники может стоять опциональный символ ;
 * - M&amp;M&APOSs декодируется в M&M's
 * - &amp;lt; декодируется в &lt;
 */
std::string HtmlDecode(std::string_view str);
std::unordered_set<size_t> GetKeysSizes(const std::unordered_map<std::string, std::string>& table);
void ToLowerCase(std::string& str);
void ToUpperCase(std::string& str);