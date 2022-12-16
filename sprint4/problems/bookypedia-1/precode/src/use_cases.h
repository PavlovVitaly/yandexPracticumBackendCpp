#pragma once

#include <string>

namespace app {

class UseCases {
public:
    virtual void AddAuthor(const std::string& name) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app
