#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

class BinaryWriter
{
public:
    BinaryWriter& operator<<(uint32 value);
    BinaryWriter& operator<<(uint64 value);
    BinaryWriter& operator<<(float value);

    BinaryWriter& WriteBytes(Span<const uint8> bytes);

    const Vector<uint8>& GetBuffer() const { return buffer; }

private:
    Vector<uint8> buffer;
};

} // namespace URay
