#include "support_types.h"

namespace model {

bool operator == (const Velocity& lhs, const Velocity& rhs) {
    return lhs.vx == rhs.vx && lhs.vy == rhs.vy;
}

}