#include "BinaryReader.h"

namespace URay
{

BinaryReader::BinaryReader(std::span<const uint8> buffer)
    : buffer(buffer), offset(0) {}

BinaryReader& BinaryReader::operator>>(uint32& value)
{
    value = *reinterpret_cast<const uint32*>(&buffer[offset]);
    offset += sizeof(uint32);
    return *this;
}

BinaryReader& BinaryReader::operator>>(uint64& value)
{
    value = *reinterpret_cast<const uint64*>(&buffer[offset]);
    offset += sizeof(uint64);
    return *this;
}

BinaryReader& BinaryReader::operator>>(float& value)
{
    value = *reinterpret_cast<const float*>(&buffer[offset]);
    offset += sizeof(float);
    return *this;
}

BinaryReader& BinaryReader::ReadBytes(std::span<uint8> bytes)
{
    std::copy(buffer.begin() + offset, buffer.begin() + offset + bytes.size(), bytes.begin());
    offset += bytes.size();
    return *this;
}

} // namespace URay
