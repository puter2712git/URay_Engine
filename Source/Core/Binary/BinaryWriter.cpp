#include "BinaryWriter.h"

namespace URay
{

BinaryWriter& BinaryWriter::operator<<(uint32 value)
{
    const uint8* bytes = reinterpret_cast<const uint8*>(&value);
    buffer.insert(buffer.end(), bytes, bytes + sizeof(value));
    return *this;
}

BinaryWriter& BinaryWriter::operator<<(uint64 value)
{
    const uint8* bytes = reinterpret_cast<const uint8*>(&value);
    buffer.insert(buffer.end(), bytes, bytes + sizeof(value));
    return *this;
}

BinaryWriter& BinaryWriter::operator<<(float value)
{
    const uint8* bytes = reinterpret_cast<const uint8*>(&value);
    buffer.insert(buffer.end(), bytes, bytes + sizeof(value));
    return *this;
}

BinaryWriter& BinaryWriter::WriteBytes(std::span<const uint8> bytes)
{
    buffer.insert(buffer.end(), bytes.begin(), bytes.end());
    return *this;
}

} // namespace URay
