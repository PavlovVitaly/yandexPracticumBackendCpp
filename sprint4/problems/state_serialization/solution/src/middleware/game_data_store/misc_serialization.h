#pragma once
#include "geom.h"

#include <boost/serialization/vector.hpp>

namespace geom {

template <typename Archive>
void serialize(Archive& ar, Point2D& point, [[maybe_unused]] const unsigned version) {
    ar& point.x;
    ar& point.y;
}

}