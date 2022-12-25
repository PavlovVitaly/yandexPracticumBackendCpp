#pragma once
#include "collision_detector.h"
#include "lost_object.h"
#include "dog.h"

#include <unordered_map>
#include <memory>

namespace model {

class ItemDogProvider: public collision_detector::ItemGathererProvider {
public:
    using Items = std::vector< std::shared_ptr<collision_detector::Item> >;
    using Dogs = std::vector< std::shared_ptr<Dog> >;

    ItemDogProvider(Items&& items, Dogs&& dogs)
        : items_(std::move(items))
        , dogs_(std::move(dogs)) {};
    virtual ~ItemDogProvider() = default;
    
    size_t ItemsCount() const override;
    collision_detector::Item GetItem(size_t idx) const override;
    size_t GatherersCount() const override;
    collision_detector::Gatherer GetGatherer(size_t idx) const override;

    const Dog::Id& GetDogId(size_t idx) const;

    template<typename T> 
    const T* const TryCastItemTo(size_t idx) const {
        return dynamic_cast<T*>(items_[idx].get());
    };

private:
    Items items_;
    Dogs dogs_;
};

}