#pragma once

#include <cstdint>
#include <limits>

namespace Soon
{
typedef std::uint32_t TypeId;
constexpr TypeId TypeIdError = std::numeric_limits<TypeId>::max();

struct Id
{
    Id(TypeId id)
    {
        _id = id;
    };

    Id(void) : _id(0){};

    TypeId GetId(void) const { return (_id); };

    bool operator==(const Id &rhs)
    {
        return (_id == rhs._id);
    }

    bool operator!=(const Id &rhs)
    {
        return (!(_id == rhs._id));
    }

    void Clear(void)
    {
        _id = 0;
    }

    TypeId _id;
};
} // namespace Soon
