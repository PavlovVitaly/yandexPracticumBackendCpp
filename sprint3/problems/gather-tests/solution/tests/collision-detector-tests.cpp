#define _USE_MATH_DEFINES
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../src/collision_detector.h"

#include <string>
#include <memory>

// Напишите здесь тесты для функции collision_detector::FindGatherEvents

namespace {
    const std::string TAG = "[FindGatherEvents]";
}

namespace collision_detector {

class ItemGathererProviderImpl: public ItemGathererProvider {
public:
    virtual ~ItemGathererProviderImpl() = default;
    
    size_t ItemsCount() const {
        return items_.size();
    };

    Item GetItem(size_t idx) const override{
        return items_[idx];
    };

    void AddItem(Item item) {
        items_.push_back(std::move(item));
    };

    size_t GatherersCount() const override{
        return gatherers_.size();
    };

    Gatherer GetGatherer(size_t idx) const override{
        return gatherers_[idx];
    };

    void AddGatherer(Gatherer gatherer) {
        gatherers_.push_back(std::move(gatherer));
    };
private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

}

using namespace std::literals;

TEST_CASE("Gather collect one item moving on x-axis", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{12.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item);
    provider.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(provider);
    CHECK(events.size() == 1);
    CHECK(events[0].sq_distance == 0);
    CHECK_THAT(events[0].time, WithinRel((item.position.x/gatherer.end_pos.x), 1e-9)); 
}