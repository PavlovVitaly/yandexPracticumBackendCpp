#include "lost_object_dog_provider.h"

namespace model{

LostObjectsDogProvider::LostObjectsDogProvider(const LostObjects& lost_objects,
                                                const Dogs& dogs) {
    for(auto [id, lost_object] : lost_objects) {
        lost_objects_.push_back(lost_object);
    }
    for(auto [id, dog] : dogs) {
        if(dog->IsFullBag()) {
            continue;
        }
        dogs_.push_back(dog);
    }
};

size_t LostObjectsDogProvider::ItemsCount() const {
    return lost_objects_.size();
};

collision_detector::Item LostObjectsDogProvider::GetItem(size_t idx) const {
    return lost_objects_[idx]->AsItem();
};

size_t LostObjectsDogProvider::GatherersCount() const {
    return dogs_.size();
};

collision_detector::Gatherer LostObjectsDogProvider::GetGatherer(size_t idx) const {
    return dogs_[idx]->AsGatherer();
};

const LostObject::Id& LostObjectsDogProvider::GetLostObjectId(size_t idx) const {
    return lost_objects_[idx]->GetId();
};

const Dog::Id& LostObjectsDogProvider::GetDogId(size_t idx) const {
    return dogs_[idx]->GetId();
};

}