#pragma once
#include <compare>

namespace util {

/**
 * ��������������� ��������� ����� "������������� ���".
 * � ��� ������� ����� ������� ������� ��� �� ������ ������� ����.
 * ������:
 *
 *  struct AddressTag{}; // ����� ���� ��� ������, �������� �����
 *  using Address = util::Tagged<std::string, AddressTag>;
 *
 *  struct NameTag{}; // ����� ���� ��� ������, �������� ���
 *  using Name = util::Tagged<std::string, NameTag>;
 *
 *  struct Person {
 *      Name name;
 *      Address address;
 *  };
 *
 *  Name name{"Harry Potter"s};
 *  Address address{"4 Privet Drive, Little Whinging, Surrey, England"s};
 *
 * Person p1{name, address}; // OK
 * Person p2{address, name}; // ������, Address � Name - ������ ����
 */
template <typename Value, typename Tag>
class Tagged {
public:
    using ValueType = Value;
    using TagType = Tag;

    explicit Tagged(Value&& v)
        : value_(std::move(v)) {
    }
    explicit Tagged(const Value& v)
        : value_(v) {
    }

    const Value& operator*() const {
        return value_;
    }

    Value& operator*() {
        return value_;
    }

    // ��� � C++20 ����� �������� �������� ��������� Tagged-�����
    // ����� ������ ������ ��������������� �������� ��� ���� value_
    auto operator<=>(const Tagged<Value, Tag>&) const = default;

private:
    Value value_;
};

// ����� ��� Tagged-����, ����� Tagged-������� ����� ���� ������� � unordered-�����������
template <typename TaggedValue>
struct TaggedHasher {
    size_t operator()(const TaggedValue& value) const {
        // ���������� ��� ��������, ����������� ������ value
        return std::hash<typename TaggedValue::ValueType>{}(*value);
    }
};

}  // namespace util
