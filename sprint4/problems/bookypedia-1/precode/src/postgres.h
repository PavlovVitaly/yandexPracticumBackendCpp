#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "author.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::work& work)
        : work_{work} {
    }

    void Save(const domain::Author& author) override;

private:
    pqxx::work& work_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

    AuthorRepositoryImpl& GetAuthors() & {
        return authors_;
    }

private:
    pqxx::connection connection_;
    pqxx::work work_{connection_};
    AuthorRepositoryImpl authors_{work_};
};

}  // namespace postgres