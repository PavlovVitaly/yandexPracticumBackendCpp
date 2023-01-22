#pragma once
#include "player_records_fwd.h"
#include "use_cases.h"

namespace db_storage {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::PlayerRecordRepository& player_records)
        : player_records_{player_records} {
    }

    void AddPlayerRecord(const domain::PlayerRecord& player_record) override;
    std::vector<domain::PlayerRecord> GetRecordsTable() override;

private:
    domain::PlayerRecordRepository& player_records_;
};

}  // namespace app
