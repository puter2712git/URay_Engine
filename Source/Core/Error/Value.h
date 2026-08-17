#pragma once

#include <string>

namespace URay
{

template <typename T>
class Value
{
public:
    Value(T value)
        : value(value)
    {
    }

public:
    T value;

    bool isValid = false;
    std::string errorMsg;
};

} // namespace URay
