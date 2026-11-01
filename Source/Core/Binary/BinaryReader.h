#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class BinaryReader
{
public:
    explicit BinaryReader(Span<const uint8> buffer);

public:
    BinaryReader& operator>>(uint32& value);
    BinaryReader& operator>>(uint64& value);
    BinaryReader& operator>>(float& value);

    BinaryReader& ReadBytes(Span<uint8> bytes);

private:
    Span<const uint8> buffer;
    uint64 offset = 0;
};

} // namespace URay
