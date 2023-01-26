#include "use_cases_impl.h"

#include <ranges>
#include <algorithm>
#include <sstream>
#include <optional>

namespace db_storage {

void UseCasesImpl::AddPlayerRecords(const std::vector<domain::PlayerRecord>& player_records) {
    player_records_.SaveRecordsTable(player_records);
};

std::vector<domain::PlayerRecord> UseCasesImpl::GetRecordsTable(size_t offset, size_t limit) {
    return player_records_.GetRecordsTable(offset, limit);
};

}  // namespace app
