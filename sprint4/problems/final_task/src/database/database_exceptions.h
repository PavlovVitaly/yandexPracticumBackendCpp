#pragma once

namespace db_ex {

class EmptyDatabaseUrl : public std::exception {
public:
    char const* what () {
        return "Empty database URL.";
    }
};

class ReturnZeroDbConnection : public std::exception {
public:
    char const* what () {
        return "Returned database connection but used zero connections.";
    }
};

}