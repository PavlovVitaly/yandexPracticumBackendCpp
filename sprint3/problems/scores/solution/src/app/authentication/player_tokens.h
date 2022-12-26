#pragma once
#include "tagged.h"
#include "player.h"

#include <random>
#include <unordered_map>
#include <memory>
#include <string>

namespace authentication {

struct TokenTag {};

using Token = util::Tagged<std::string, TokenTag>;
using TokenHasher = util::TaggedHasher<Token>;

class PlayerTokens {
public:
    PlayerTokens() = default;
    PlayerTokens(const PlayerTokens& other) = default;
    PlayerTokens(PlayerTokens&& other) = default;
    PlayerTokens& operator = (const PlayerTokens& other) = default;
    PlayerTokens& operator = (PlayerTokens&& other) = default;
    virtual ~PlayerTokens() = default;

    Token AddPlayer(std::shared_ptr<app::Player> player);
    std::shared_ptr<app::Player> FindPlayerBy(Token token);
private:
    std::unordered_map< Token, std::shared_ptr<app::Player>, TokenHasher > tokenToPalyer_;
    std::random_device random_device_;
    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    // Чтобы сгенерировать токен, получите из generator1_ и generator2_
    // два 64-разрядных числа и, переведя их в hex-строки, склейте в одну.
    // Вы можете поэкспериментировать с алгоритмом генерирования токенов,
    // чтобы сделать их подбор ещё более затруднительным
}; 

}  // namespace authentication