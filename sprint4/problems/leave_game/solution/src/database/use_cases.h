#pragma once
#include "player_record.h"

#include <string>
#include <vector>

namespace db_storage {

class UseCases {
public:
    virtual void AddPlayerRecord(const domain::PlayerRecord& player_record) = 0;
    virtual std::vector<domain::PlayerRecord> GetRecordsTable() = 0;

protected:
    ~UseCases() = default;
};

}  // namespace domain
