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
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item.position.x/gatherer.end_pos.x), 1e-9)); 
}

TEST_CASE("Gather collect one item moving on x-axis on edge", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{12.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {12.5, 0}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item);
    provider.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(provider);
    
    CHECK(events.size() == 1);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item.position.x/gatherer.end_pos.x), 1e-9)); 
}

TEST_CASE("Gather collect one item moving on x-axis on side", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{12.5, 0.5}, 0.0};
    collision_detector::Gatherer gatherer{{0, 0.1}, {22.5, 0.1}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item);
    provider.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(provider);
    
    CHECK(events.size() == 1);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.16, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item.position.x/gatherer.end_pos.x), 1e-9)); 
}

TEST_CASE("Gather collect one item moving on y-axis", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item{{0, 12.5}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {0, 22.5}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item);
    provider.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(provider);
    
    CHECK(events.size() == 1);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item.position.y/gatherer.end_pos.y), 1e-9)); 
}

TEST_CASE("Gather collect two unordered items moving on x-axis", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{12.5, 0}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item1);
    provider.AddItem(item2);
    provider.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(provider);
    
    CHECK(events.size() == 2);

    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer.end_pos.x), 1e-9)); 
    
    CHECK(events[1].item_id == 0);
    CHECK(events[1].gatherer_id == 0);
    CHECK_THAT(events[1].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[1].time, WithinRel((item1.position.x/gatherer.end_pos.x), 1e-9)); 
}

TEST_CASE("Gather collect one of two items moving on x-axis", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{42.5, 0}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item1);
    provider.AddItem(item2);
    provider.AddGatherer(gatherer);
    auto events = collision_detector::FindGatherEvents(provider);

    CHECK(events.size() == 1);

    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer.end_pos.x), 1e-9)); 
}

TEST_CASE("Two gathers collect two separate items moving on x-axis and y-axis", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{0, 12.5}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer1{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::Gatherer gatherer2{{0, 0}, {0, 22.5}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item1);
    provider.AddItem(item2);
    provider.AddGatherer(gatherer1);
    provider.AddGatherer(gatherer2);
    auto events = collision_detector::FindGatherEvents(provider);
    
    CHECK(events.size() == 2);

    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer1.end_pos.x), 1e-9)); 
    
    CHECK(events[1].item_id == 0);
    CHECK(events[1].gatherer_id == 1);
    CHECK_THAT(events[1].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[1].time, WithinRel((item1.position.y/gatherer2.end_pos.y), 1e-9)); 
}

TEST_CASE("Two gathers collect three items moving on x-axis and y-axis", TAG) {
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{{12.5, 0}, 0.6};
    collision_detector::Item item2{{6.5, 0}, 0.6};
    collision_detector::Gatherer gatherer1{{0, 0}, {22.5, 0}, 0.6};
    collision_detector::Gatherer gatherer2{{0, 0}, {10, 0}, 0.6};
    collision_detector::ItemGathererProviderImpl provider;
    provider.AddItem(item1);
    provider.AddItem(item2);
    provider.AddGatherer(gatherer1);
    provider.AddGatherer(gatherer2);
    auto events = collision_detector::FindGatherEvents(provider);
    
    CHECK(events.size() == 3);

    CHECK(events[0].item_id == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[0].time, WithinRel((item2.position.x/gatherer1.end_pos.x), 1e-9)); 

    CHECK(events[1].item_id == 0);
    CHECK(events[1].gatherer_id == 0);
    CHECK_THAT(events[1].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[1].time, WithinRel((item1.position.x/gatherer1.end_pos.x), 1e-9)); 

    CHECK(events[2].item_id == 1);
    CHECK(events[2].gatherer_id == 1);
    CHECK_THAT(events[2].sq_distance, WithinRel(0.0, 1e-9));
    CHECK_THAT(events[2].time, WithinRel((item2.position.x/gatherer2.end_pos.x), 1e-9)); 
}