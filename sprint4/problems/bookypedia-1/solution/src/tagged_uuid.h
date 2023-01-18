#pragma once
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <string>

#include "tagged.h"

namespace util {

namespace detail {

using UUIDType = boost::uuids::uuid;

UUIDType NewUUID();
constexpr UUIDType ZeroUUID{{0}};

std::string UUIDToString(const UUIDType& uuid);
UUIDType UUIDFromString(std::string_view str);

}  // namespace detail

template <typename Tag>
class TaggedUUID : public Tagged<detail::UUIDType, Tag> {
public:
    using Base = Tagged<detail::UUIDType, Tag>;
    using Tagged<detail::UUIDType, Tag>::Tagged;

    TaggedUUID()
        : Base{detail::ZeroUUID} {
    }

    static TaggedUUID New() {
        return TaggedUUID{detail::NewUUID()};
    }

    static TaggedUUID FromString(const std::string& uuid_as_text) {
        return TaggedUUID{detail::UUIDFromString(uuid_as_text)};
    }


    std::string ToString() const {
        return detail::UUIDToString(**this);
    }
};

}  // namespace util