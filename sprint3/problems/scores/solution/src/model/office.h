#pragma once
#include "tagged.h"
#include "support_types.h"
#include "collision_detector.h"
#include "model_invariants.h"

#include <cmath>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace model {

class Office: public collision_detector::Item  {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, geom::Point2D position, Offset offset, double width = BASE_WIDTH) noexcept
        : Item(position, width)
        , id_{std::move(id)}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Offset offset_;
};

}