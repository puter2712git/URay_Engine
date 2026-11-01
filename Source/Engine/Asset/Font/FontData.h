#pragma once

#include <Core/Type/TypeAlias.h>

#include <vector>

namespace URay
{

class BinaryReader;
class BinaryWriter;

struct FontData
{
    Vector<uint8> bytes;

    bool Serialize(BinaryWriter& writer) const;
    bool Deserialize(BinaryReader& reader);
};

} // namespace URay
