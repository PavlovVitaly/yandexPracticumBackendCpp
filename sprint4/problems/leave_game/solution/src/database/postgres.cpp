#include "postgres.h"

#include <string>
#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void PlayerRecordRepositoryImpl::Save(const domain::PlayerRecord& player_record) {
    auto conn = connection_pool_.GetConnection();
    pqxx::work work_{*conn};
    work_.exec_params(R"(
INSERT INTO records (name, score, play_time) VALUES ($1, $2, $3);
)"_zv,
    player_record.GetName(),
    player_record.GetScore(),
    player_record.GetPlayTime());
    work_.commit();
};

std::vector<domain::PlayerRecord> PlayerRecordRepositoryImpl::GetRecordsTable() {
    auto conn = connection_pool_.GetConnection();
    std::vector<domain::PlayerRecord> records_table;
    pqxx::read_transaction read_transaction_{*conn};
    auto query_text = "SELECT name, score, play_time FROM records ORDER BY score DESC, play_time ASC, name ASC"_zv;
    for (auto [name, score, play_time] : read_transaction_.query<std::string, size_t, int64_t>(query_text)) {
        records_table.emplace_back(name, score, play_time);
    }
    return records_table;
};

Database::Database(const db_conn_settings::DbConnectrioSettings& db_settings)
    : connection_pool_{
        db_settings.number_of_connection,
        [db_url = std::move(db_settings.db_url)]() {
            return std::make_shared<pqxx::connection>(db_url);
        }
    } {
    auto conn = connection_pool_.GetConnection();
    pqxx::work work_{*conn};
    work_.exec(R"(
CREATE TABLE IF NOT EXISTS records (
    id SERIAL PRIMARY KEY,
    name varchar(40) NOT NULL,
    score integer CONSTRAINT score_positive CHECK (score >= 0),
    play_time integer NOT NULL CONSTRAINT play_time_positive CHECK (play_time >= 0)     
);
CREATE INDEX IF NOT EXISTS records_score ON records (score); 
)"_zv);
    // коммитим изменения
    work_.commit();
}

}  // namespace postgres