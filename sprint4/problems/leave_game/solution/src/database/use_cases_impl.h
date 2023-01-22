#pragma once
#include "use_cases.h"

namespace db_storage {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::PlayerRecordRepository& player_records)
        : player_records_{player_records} {
    }

    void AddPlayerRecords(const std::vector<domain::PlayerRecord>& player_records) override;
    std::vector<domain::PlayerRecord> GetRecordsTable(size_t offset, size_t limit) override;

private:
    domain::PlayerRecordRepository& player_records_;
};

}  // namespace app
