#include "use_cases_impl.h"

#include <ranges>
#include <algorithm>
#include <sstream>
#include <optional>

namespace db_storage {

void UseCasesImpl::AddPlayerRecord(const domain::PlayerRecord& player_record) {
    player_records_.Save(player_record);
};

std::vector<domain::PlayerRecord> UseCasesImpl::GetRecordsTable() {
    return player_records_.GetRecordsTable();
};

}  // namespace app
