#pragma once

#include <Core/Type/TypeAlias.h>

#include <span>
#include <vector>

namespace URay
{

class BinaryReader
{
public:
    explicit BinaryReader(std::span<const uint8> buffer);

public:
    BinaryReader& operator>>(uint32& value);
    BinaryReader& operator>>(uint64& value);
    BinaryReader& operator>>(float& value);

    BinaryReader& ReadBytes(std::span<uint8> bytes);

private:
    std::span<const uint8> buffer;
    uint64 offset = 0;
};

} // namespace URay
