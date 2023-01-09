#pragma once
#include "tagged.h"
#include "road.h"
#include "support_types.h"


#include <cmath>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace model {

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

}