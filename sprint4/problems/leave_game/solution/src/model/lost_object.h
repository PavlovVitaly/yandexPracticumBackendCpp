#pragma once
#include "support_types.h"
#include "tagged.h"
#include "geom.h"
#include "collision_detector.h"
#include "model_invariants.h"

namespace model {

class LostObject: public collision_detector::Item {
  inline static size_t max_id_cont_ = 0;
public:
  using Id = util::Tagged<size_t, LostObject>;

  LostObject(double width = LOOT_WIDTH)
        : Item({0,0}, width)
        , id_(Id{LostObject::max_id_cont_++}) {};
  const Id& GetId() const;
  void SetId(const Id& id);
  size_t GetType() const;
  void SetType(size_t type);
  size_t GetValue() const;
  void SetValue(size_t value);
private:
  Id id_;
  size_t type_;
  size_t value_{0};
};

struct LostObjectHasher {
  size_t operator()(const LostObject& lost_object) const{
    size_t sd = 17;
    return std::hash<size_t>{}(lost_object.GetType())*sd*sd +
            std::hash<double>{}(lost_object.GetPosition().x)*sd +
            std::hash<double>{}(lost_object.GetPosition().y);
  }
};

}