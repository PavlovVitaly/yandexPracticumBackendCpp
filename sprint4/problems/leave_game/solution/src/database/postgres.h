#pragma once
#include "player_record.h"
#include "connection_pool.h"
#include "database_connection_settings.h"

#include <pqxx/connection>
#include <pqxx/transaction>

namespace postgres {

class PlayerRecordRepositoryImpl : public domain::PlayerRecordRepository {
public:
    explicit PlayerRecordRepositoryImpl(db::ConnectionPool& connection_pool)
        : connection_pool_(connection_pool) {
    };

    void Save(const domain::PlayerRecord& player_record) override;
    std::vector<domain::PlayerRecord> GetRecordsTable() override;

private:
    db::ConnectionPool& connection_pool_;
};

class Database {
public:
    explicit Database(const db_conn_settings::DbConnectrioSettings& db_settings);

    PlayerRecordRepositoryImpl& GetPlayerRecords() & {
        return player_records_;
    }

private:
    db::ConnectionPool connection_pool_;
    PlayerRecordRepositoryImpl player_records_{connection_pool_};
};

}  // namespace postgres