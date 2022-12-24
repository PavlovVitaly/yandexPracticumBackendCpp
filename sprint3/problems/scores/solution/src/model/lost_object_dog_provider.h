#pragma once
#include "collision_detector.h"
#include "lost_object.h"
#include "dog.h"

#include <unordered_map>
#include <memory>

namespace model {

class LostObjectsDogProvider: public collision_detector::ItemGathererProvider {
public:
    using LostObjectIdHasher = util::TaggedHasher<LostObject::Id>;
    using LostObjects = std::unordered_map<LostObject::Id,
                                            std::shared_ptr<LostObject>,
                                            LostObjectIdHasher>;
    using DogIdHasher = util::TaggedHasher<Dog::Id>;
    using Dogs = std::unordered_map<Dog::Id,
                                    std::shared_ptr<Dog>,
                                    DogIdHasher>;

    LostObjectsDogProvider(const LostObjects& lost_objects,
                            const Dogs& dogs);
    virtual ~LostObjectsDogProvider() = default;
    
    size_t ItemsCount() const override;
    collision_detector::Item GetItem(size_t idx) const override;
    size_t GatherersCount() const override;
    collision_detector::Gatherer GetGatherer(size_t idx) const override;

    const LostObject::Id& GetLostObjectId(size_t idx) const;
    const Dog::Id& GetDogId(size_t idx) const;

private:
    std::vector< std::shared_ptr<LostObject> > lost_objects_;
    std::vector< std::shared_ptr<Dog> > dogs_;
};

}