#pragma once

namespace db_ex {

class EmptyDatabaseUrl : public std::exception {
public:
    char const* what () {
        return "Empty database URL.";
    }
};

}