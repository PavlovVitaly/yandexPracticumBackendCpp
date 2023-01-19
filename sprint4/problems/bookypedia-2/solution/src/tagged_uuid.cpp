#include "tagged_uuid.h"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace util {
namespace detail {

UUIDType NewUUID() {
    return boost::uuids::random_generator()();
}

std::string UUIDToString(const UUIDType& uuid) {
    return to_string(uuid);
}

UUIDType UUIDFromString(std::string_view str) {
    boost::uuids::string_generator gen;
    return gen(str.begin(), str.end());
}

}  // namespace detail
}  // namespace util
