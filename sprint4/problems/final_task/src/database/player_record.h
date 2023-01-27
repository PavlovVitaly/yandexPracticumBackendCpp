#pragma once

#include <string>
#include <vector>

namespace domain {

class PlayerRecord {
public:
    PlayerRecord(std::string name, size_t score, int64_t play_time)
        : name_(std::move(name))
        , score_(score)
        , play_time_(play_time) {

    };

    const std::string& GetName() const noexcept {
        return name_;
    }

    size_t GetScore() const noexcept {
        return score_;
    }

    int64_t GetPlayTime() const noexcept {
        return play_time_;
    }

private:
    std::string name_{};
    size_t score_{0};
    int64_t play_time_{0};
};


class PlayerRecordRepository {
public:
    virtual void SaveRecordsTable(const std::vector<domain::PlayerRecord>& player_records) = 0;
    virtual std::vector<PlayerRecord> GetRecordsTable(size_t offset, size_t limit) = 0;

protected:
    ~PlayerRecordRepository() = default;
};

}