#include "collision_detector.h"
#include "physics_exceptions.h"

#include <cassert>

namespace collision_detector {

CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c) {
    // Проверим, что перемещение ненулевое.
    // Тут приходится использовать строгое равенство, а не приближённое,
    // пскольку при сборе заказов придётся учитывать перемещение даже на небольшое
    // расстояние.
    if(!(b.x != a.x || b.y != a.y)) {
        throw collision_detector_ex::ZeroMove(a, b);
    };
    const double u_x = c.x - a.x;
    const double u_y = c.y - a.y;
    const double v_x = b.x - a.x;
    const double v_y = b.y - a.y;
    const double u_dot_v = u_x * v_x + u_y * v_y;
    const double u_len2 = u_x * u_x + u_y * u_y;
    const double v_len2 = v_x * v_x + v_y * v_y;
    const double proj_ratio = u_dot_v / v_len2;
    const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

    return CollectionResult(sq_distance, proj_ratio);
}

// В задании на разработку тестов реализовывать следующую функцию не нужно -
// она будет линковаться извне.

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider) {\
    std::vector<GatheringEvent> result;
    for(size_t g = 0; g < provider.GatherersCount(); ++g) {
        for(size_t i = 0; i < provider.ItemsCount(); ++i) {
            auto gatherer = provider.GetGatherer(g);
            auto item = provider.GetItem(i);
            if((!(gatherer.end_pos.x != gatherer.start_pos.x || gatherer.end_pos.y != gatherer.start_pos.y))) {
                continue;
            }
            auto collect_result = TryCollectPoint(gatherer.start_pos, gatherer.end_pos, item.GetPosition());
            if(collect_result.IsCollected(item.GetWidth() + gatherer.width)){
                result.push_back({i, g, collect_result.sq_distance, collect_result.proj_ratio});
            }
        }
    }
    sort(result.begin(), result.end(),
        [](const GatheringEvent& lhs, const GatheringEvent& rhs) {
        return lhs.time < rhs.time;
    });
    return result;
}


}  // namespace collision_detector
