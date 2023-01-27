#pragma once
#include "geom.h"

#include <string>
#include <sstream>

namespace collision_detector_ex {

class ZeroMove : public std::exception {
public:
    ZeroMove(
        const geom::Point2D& start,
        const geom::Point2D& end)
        : start_(start)
        , end_(end) {
    }

    char const* what () {
        std::stringstream ss;
        ss << "Zero move. ";
        ss << "Start x = " << start_.x << " , y = " << start_.y << " . ";
        ss << "End x = " << end_.x << " , y = " << end_.y << " .";
        return ss.str().c_str();
    }

private:
    const geom::Point2D start_;
    const geom::Point2D end_;
};

}