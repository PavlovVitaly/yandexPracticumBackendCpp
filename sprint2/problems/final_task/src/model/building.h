#pragma once
#include "tagged.h"
#include "road.h"
#include "support_types.h"


#include <cmath>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <boost/json.hpp>

namespace model {

namespace json = boost::json;

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Building& building);
Building tag_invoke(json::value_to_tag<Building>, const json::value& jv);

}