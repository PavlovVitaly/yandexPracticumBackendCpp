#pragma once
#include "support_types.h"
#include "tagged.h"
#include "geom.h"

namespace model {

class LostObject {
  inline static size_t max_id_cont_ = 0;
public:
  using Id = util::Tagged<size_t, LostObject>;

  LostObject() : 
        id_(Id{LostObject::max_id_cont_++}) {};
  const Id& GetId() const;
  size_t GetType() const;
  void SetType(size_t type);
  const geom::Point2D& GetPosition() const;
  void SetPosition(geom::Point2D position);
private:
  Id id_;
  size_t type_;
  geom::Point2D position_;
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