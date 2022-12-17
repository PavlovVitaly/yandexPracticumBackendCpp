#pragma once
#include "tagged.h"
#include "support_types.h"

#include <cmath>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <boost/json.hpp>

namespace model {

namespace json = boost::json;

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Office& office);
Office tag_invoke(json::value_to_tag<Office>, const json::value& jv);

}