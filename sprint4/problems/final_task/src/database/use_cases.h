#pragma once
#include "player_record.h"

#include <string>
#include <vector>

namespace db_storage {

class UseCases {
public:
    virtual void AddPlayerRecords(const std::vector<domain::PlayerRecord>& player_records) = 0;
    virtual std::vector<domain::PlayerRecord> GetRecordsTable(size_t offset, size_t limit) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace domain
