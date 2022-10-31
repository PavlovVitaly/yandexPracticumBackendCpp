#pragma once
#include <array>
#include <cassert>
#include <optional>
#include <iostream>
#include <random>
#include <set>

class SeabattleField {
public:
    enum class State {
        UNKNOWN,
        EMPTY,
        KILLED,
        SHIP
    };

    static const size_t field_size = 8;

    SeabattleField(State default_elem = State::UNKNOWN) {
        field_.fill(default_elem);
    }

    template <class T>
    static SeabattleField GetRandomField(T&& random_engine) {
        std::optional<SeabattleField> res;
        do {
            res = TryGetRandomField(random_engine);
        } while (!res);

        return *res;
    }

private:
    template<class T>
    static std::optional<SeabattleField> TryGetRandomField(T&& random_engine) {
        SeabattleField result{State::EMPTY};

        std::set<std::pair<size_t, size_t>> availableElements;
        std::vector ship_sizes = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};

        const auto mark_cell_as_used = [&](size_t x, size_t y) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    availableElements.erase({x + dx, y + dy});
                }
            }
        };

        const auto dir_to_dxdy = [](size_t dir) -> std::pair<int, int> {
            int dx = dir == 1 ? 1 : dir == 3 ? -1 : 0;
            int dy = dir == 0 ? 1 : dir == 2 ? -1 : 0;

            return {dx, dy};
        };

        const auto check_ship_available = [&](size_t x, size_t y, size_t dir, int ship_length) {
            auto [dx, dy] = dir_to_dxdy(dir);

            for (int i = 0; i < ship_length; ++i) {
                size_t cx = x + dx * i;
                size_t cy = y + dy * i;

                if (cx >= field_size || cy >= field_size) {
                    return false;
                }
                if (availableElements.count({cx, cy}) == 0) {
                    return false;
                }
            }

            return true;
        };

        const auto mark_ship = [&](size_t x, size_t y, size_t dir, int ship_length){
            auto [dx, dy] = dir_to_dxdy(dir);

            for (int i = 0; i < ship_length; ++i) {
                size_t cx = x + dx * i;
                size_t cy = y + dy * i;

                result.Get(cx, cy) = State::SHIP;
                mark_cell_as_used(cx, cy);
            }
        };

        for (size_t y = 0; y < field_size; ++y) {
            for (size_t x = 0; x < field_size; ++x) {
                availableElements.insert({x, y});
            }
        }

        using Distr = std::uniform_int_distribution<size_t>;
        using Param = Distr::param_type;

        static const int max_attempts = 100;

        for (int length : ship_sizes) {
            std::pair<size_t, size_t> pos;
            size_t direction;
            int attempt = 0;

            Distr d;
            do {
                if (attempt++ >= max_attempts || availableElements.empty()) {
                    return std::nullopt;
                }

                size_t pos_index = d(random_engine, Param(0, availableElements.size() - 1));
                direction = d(random_engine, Param(0, 3));
                pos = *std::next(availableElements.begin(), pos_index);
            } while (!check_ship_available(pos.first, pos.second, direction, length));
            mark_ship(pos.first, pos.second, direction, length);
        }

        return result;
    }

    bool IsKilledInDirection(size_t x, size_t y, int dx, int dy) const {
        for (; x < field_size && y < field_size; x += dx, y += dy) {
            if (Get(x, y) == State::EMPTY) {
                return true;
            }
            if (Get(x, y) != State::KILLED) {
                return false;
            }
        }
        return true;
    }

    void MarkKillInDirection(size_t x, size_t y, int dx, int dy) {
        auto mark_cell_empty = [this](size_t x, size_t y) {
            if (x >= field_size || y >= field_size) {
                return;
            }
            if (Get(x, y) != State::UNKNOWN) {
                return;
            }
            Get(x, y) = State::EMPTY;
        };
        for (; x < field_size && y < field_size; x += dx, y += dy) {
            mark_cell_empty(x + dy, y + dx);
            mark_cell_empty(x - dy, y - dx);
            mark_cell_empty(x, y);
            if (Get(x, y) != State::KILLED) {
                return;
            }
        }
    }

public:
    enum class ShotResult {
        MISS = 0,
        HIT  = 1,
        KILL = 2
    };

    ShotResult Shoot(size_t x, size_t y) {
        if (Get(x, y) != State::SHIP) return ShotResult::MISS;

        Get(x, y) = State::KILLED;
        --weight_;

        return IsKilled(x, y) ? ShotResult::KILL : ShotResult::HIT;
    }

    void MarkMiss(size_t x, size_t y) {
        if (Get(x, y) != State::UNKNOWN) {
            return;
        }
        Get(x, y) = State::EMPTY;
    }

    void MarkHit(size_t x, size_t y) {
        if (Get(x, y) != State::UNKNOWN) {
            return;
        }
        --weight_;
        Get(x, y) = State::KILLED;
    }

    void MarkKill(size_t x, size_t y) {
        if (Get(x, y) != State::UNKNOWN) {
            return;
        }
        MarkHit(x, y);
        MarkKillInDirection(x, y, 1, 0);
        MarkKillInDirection(x, y, -1, 0);
        MarkKillInDirection(x, y, 0, 1);
        MarkKillInDirection(x, y, 0, -1);
    }

    State operator()(size_t x, size_t y) const {
        return Get(x, y);
    }

    bool IsKilled(size_t x, size_t y) const {
        return IsKilledInDirection(x, y, 1, 0) && IsKilledInDirection(x, y, -1, 0)
            && IsKilledInDirection(x, y, 0, 1) && IsKilledInDirection(x, y, 0, -1);
    }

    static void PrintDigitLine(std::ostream& out) {
        out << "  1 2 3 4 5 6 7 8  ";
    }

    void PrintLine(std::ostream& out, size_t y) const {
        std::array<char, field_size * 2 - 1> line;
        for (size_t x = 0; x < field_size; ++x) {
            line[x * 2] = Repr((*this)(x, y));
            if (x + 1 < field_size) {
                line[x * 2 + 1] = ' ';
            }
        }

        char line_char = static_cast<char>('A' + y);

        out.put(line_char);
        out.put(' ');
        out.write(line.data(), line.size());
        out.put(' ');
        out.put(line_char);
    }

    bool IsLoser() const {
        return weight_ == 0;
    }

private:
    State& Get(size_t x, size_t y) {
        return field_[x + y * field_size];
    }

    State Get(size_t x, size_t y) const {
        return field_[x + y * field_size];
    }

    static char Repr(State state) {
        switch (state) {
            case State::UNKNOWN:
                return '?';
            case State::EMPTY:
                return '.';
            case State::SHIP:
                return 'o';
            case State::KILLED:
                return 'x';
        }

        return '\0';
    }

private:
    std::array<State, field_size * field_size> field_;
    int weight_ = 1 * 4 + 2 * 3 + 3 * 2 + 4 * 1;
};
