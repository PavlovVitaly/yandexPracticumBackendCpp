#include "game_session.h"
#include "random_generators.h"
#include "support_types.h"

#include <ranges>
#include <algorithm>

namespace app {

void GameSession::Run() {
    if(period_of_update_game_state_.count() != 0){
        update_game_state_ticker_ = std::make_shared<time_m::Ticker>(
            strand_,
            period_of_update_game_state_,
            [self = shared_from_this()](const TimeInterval& delta_time) {
                self->UpdateGameState(delta_time);
            }
        );
        update_game_state_ticker_->Start();
    }
    generate_loot_ticker_ = std::make_shared<time_m::Ticker>(
        strand_,
        loot_generator_.GetPeriod(),
        [self = shared_from_this()](const TimeInterval& delta_time) {
                self->GenerateLoot(delta_time);
        }
    );
    generate_loot_ticker_->Start();
};

const GameSession::Id& GameSession::GetId() const noexcept {
    return id_;
}

const std::shared_ptr<const model::Map> GameSession::GetMap() {
    return map_;
};

std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
    return strand_;
};

std::weak_ptr<model::Dog> GameSession::CreateDog(const std::string& dog_name, const model::Map& map, bool randomize_spawn_points){
    auto dog = std::make_shared<model::Dog>(dog_name, map_->GetBagCapacity());
    if(randomize_spawn_points) {
        LocateDogInRandomPositionOnMap(dog);
    } else {
        LocateDogInStartPointOnMap(dog);
    }
    dogs_[dog->GetId()] = dog;
    net::dispatch(*strand_, [self = shared_from_this()]{
        self->GenerateLoot(self->loot_generator_.GetPeriod());
    });

    return dog;
};

const GameSession::LostObjects& GameSession::GetLostObjects() {
    return lost_objects_;
};

void GameSession::AddLostObject(model::LostObject lost_object) {
    auto lost_obj = std::make_shared<model::LostObject>(std::move(lost_object));
    lost_objects_[lost_obj->GetId()] = lost_obj;
};

void GameSession::UpdateGameState(const GameSession::TimeInterval& delta_time) {
    for(auto [dog_id, dog] : dogs_) {
        auto [new_position, new_velocity] = map_->GetValidMove(
            dog->GetPosition(),
            dog->CalculateNewPosition(delta_time),
            dog->GetVelocity()
        );
        dog->MakeDogAction(new_position, new_velocity, delta_time);
    }
    HandleLoot();
    RemoveInactiveDogs();
};

void GameSession::GenerateLoot(const GameSession::TimeInterval& delta_time) {
    auto num_new_lost_obj = loot_generator_.Generate(delta_time,
                                                    lost_objects_.size(),
                                                    dogs_.size());
    for(size_t i = 0; i < num_new_lost_obj; ++i) {
        CreateLostObject();
    }
};

void GameSession::CreateLostObject() {
    auto lost_obj = std::make_shared<model::LostObject>();
    SetRandomLootType(lost_obj);
    LocateLootInRandomPositionOnMap(lost_obj);
    size_t value = map_->GetLootTypeBy(lost_obj->GetType()).value;
    lost_obj->SetValue(value);
    lost_objects_[lost_obj->GetId()] = lost_obj;
};

void GameSession::SetRandomLootType(std::shared_ptr<model::LostObject> loot) {
    auto type = utils::GenerateSizeTFromInterval(0, map_->GetNumberOfLootTypes() - 1);
    loot->SetType(type);
};

void GameSession::LocateLootInRandomPositionOnMap(std::shared_ptr<model::LostObject> loot) {
    loot->SetPosition(map_->GenerateRandomPosition());
};

void GameSession::LocateDogInRandomPositionOnMap(std::shared_ptr<model::Dog> dog) {
    dog->SetPosition(map_->GenerateRandomPosition());
};

void GameSession::LocateDogInStartPointOnMap(std::shared_ptr<model::Dog> dog) {
    auto roads = map_->GetRoads();
    auto road = roads[0];
    dog->SetPosition({static_cast<double>(road.GetStart().x),
                        static_cast<double>(road.GetStart().y)});
};

void GameSession::HandleLoot() {
    std::vector< std::shared_ptr<collision_detector::Item> > items;
    std::vector< std::shared_ptr<model::Dog> > dogs;
    for(auto [id, lost_obj] : lost_objects_) {
        items.push_back(lost_obj);
    }
    for(auto office : map_->GetOffices()) {
        items.push_back(std::make_shared<model::Office>(office));
    }
    for(auto [id, dog] : dogs_) {
        dogs.push_back(dog);
    }
    model::ItemDogProvider provider(std::move(items), std::move(dogs));
    auto collected_loot = collision_detector::FindGatherEvents(provider);
    if(collected_loot.empty()) {
        return;
    }
    for(auto clltd_loot : collected_loot){
        CollectLoot(provider, clltd_loot.item_id, clltd_loot.gatherer_id);
        DropLoot(provider, clltd_loot.item_id, clltd_loot.gatherer_id);
    }
};

void GameSession::CollectLoot(const model::ItemDogProvider& provider,
                    size_t item_id,
                    size_t gatherer_id) {
    auto dog = dogs_[provider.GetDogId(gatherer_id)];
    const auto* const casted_lost_obj = provider.TryCastItemTo<model::LostObject>(item_id);
    if(casted_lost_obj) {
        auto lost_object_id = casted_lost_obj->GetId();
        if(dog->IsFullBag() || !lost_objects_.contains(lost_object_id)) {
            return;
        }
        auto loot = lost_objects_.at(lost_object_id);
        dog->CollectLostObject(loot);
        std::erase_if(lost_objects_, [id = lost_object_id](const auto& item) {
            auto const& [key, value] = item;
            return key == id;
        });
    }
};

void GameSession::DropLoot(const model::ItemDogProvider& provider, size_t item_id, size_t gatherer_id) {
    auto dog = dogs_[provider.GetDogId(gatherer_id)];
    const auto* const casted_office = provider.TryCastItemTo<model::Office>(item_id);
    if(casted_office) {
        if(dog->IsEmptyBag()) {
            return;
        }
        auto office_id = casted_office->GetId();
        dog->DropLostObjectsFromBag();
    }
};

void GameSession::AddDog(std::shared_ptr<model::Dog> dog) {
    dogs_[dog->GetId()] = dog;
};

void GameSession::AddRemoveInactivePlayersHandler(
        std::function<void(const GameSession::Id&)> handler) {
    remove_inactive_players_sig.connect(handler);
};

void GameSession::AddHandlingFinishedPlayersEvent(
    std::function<void(const std::vector<domain::PlayerRecord>&)> handler) {
    handle_finished_players_sig.connect(handler);
};

void GameSession::RemoveInactiveDogs() {
    std::vector<domain::PlayerRecord> player_records;

    std::ranges::copy(
        dogs_
        | std::views::values
        | std::views::filter(
            [](auto dog) {
                return static_cast<bool>(dog->GetPlayTime());
            }
        ) | std::views::transform(
            [](auto dog)->domain::PlayerRecord {
                return domain::PlayerRecord{dog->GetName(),
                                        dog->GetScore(),
                                        dog->GetPlayTime().value().count()};
            }
        ), std::back_inserter(player_records)
    );

    if(player_records.empty()) {
        return;
    }

    std::erase_if(dogs_, [](const auto& item) {
        auto const& [dog_id, dog] = item;
        return dog->GetPlayTime();
    });
    handle_finished_players_sig(std::move(player_records));
    remove_inactive_players_sig(id_);
};

}