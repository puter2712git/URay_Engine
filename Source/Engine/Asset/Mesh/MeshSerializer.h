#pragma once

#include "Engine/Asset/Mesh/MeshCookData.h"

#include <string>
#include <vector>

namespace URay
{

class MeshSerializer
{
public:
    std::vector<uint8> Serialize(const MeshCookData& data) const;
    bool Deserialize(const std::vector<uint8>& bytes, MeshCookData& data) const;

private:
    void WriteBytes(std::vector<uint8>& bytes, uint32 value) const;
    uint32 ReadBytes(const std::vector<uint8>& bytes, size_t offset) const;

    void WriteUInt64(std::vector<uint8>& bytes, uint64 value) const;
    uint64 ReadUInt64(const std::vector<uint8>& bytes, size_t offset) const;
    void WriteString(std::vector<uint8>& bytes, const std::string& value) const;

    void WriteFloat(std::vector<uint8>& bytes, float value) const;
    float ReadFloat(const std::vector<uint8>& bytes, size_t offset) const;
};

} // namespace URay
