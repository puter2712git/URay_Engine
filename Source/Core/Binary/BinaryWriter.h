#pragma once

#include <Core/Type/Types.h>

#include <span>
#include <vector>

namespace URay
{

class BinaryWriter
{
public:
    BinaryWriter& operator<<(uint32 value);
    BinaryWriter& operator<<(uint64 value);
    BinaryWriter& operator<<(float value);

    BinaryWriter& WriteBytes(std::span<const uint8> bytes);

    const std::vector<uint8>& GetBuffer() const { return buffer; }

private:
    std::vector<uint8> buffer;
};

} // namespace URay
