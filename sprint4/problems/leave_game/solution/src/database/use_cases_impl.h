#pragma once
#include "use_cases.h"
#include "database_invariants.h"

namespace db_storage {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::PlayerRecordRepository& player_records)
        : player_records_{player_records} {
    }

    void AddPlayerRecords(const std::vector<domain::PlayerRecord>& player_records) override;
    std::vector<domain::PlayerRecord> GetRecordsTable(
        size_t offset = db_invariants::DEFAULT_OFFSET,
        size_t limit = db_invariants::DEFAULT_LIMIT
    ) override;

private:
    domain::PlayerRecordRepository& player_records_;
};

}  // namespace app
