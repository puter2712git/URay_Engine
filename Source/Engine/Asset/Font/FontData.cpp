#include "FontData.h"

#include <Core/Binary/BinaryReader.h>
#include <Core/Binary/BinaryWriter.h>

namespace URay
{

bool FontData::Serialize(BinaryWriter& writer) const
{
    const uint64 dataSize = static_cast<uint64>(bytes.size());
    writer << dataSize;
    writer.WriteBytes(std::span<const uint8>(bytes));

    return true;
}

bool FontData::Deserialize(BinaryReader& reader)
{
    uint64 dataSize;
    reader >> dataSize;

    bytes.resize(static_cast<size_t>(dataSize));
    reader.ReadBytes(std::span<uint8>(bytes));

    return true;
}

} // namespace URay