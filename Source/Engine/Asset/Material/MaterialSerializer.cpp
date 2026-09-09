#include "MaterialSerializer.h"

#include <bit>
#include <limits>
#include <unordered_set>

namespace URay
{

namespace
{
constexpr uint32 MaterialCookMagic = 0x4C54414D; // MATL
constexpr uint32 MaterialCookVersion = 2;

bool IsValidParameterType(uint8 value)
{
    return value <= static_cast<uint8>(MaterialCookParameterType::Texture2D);
}
} // namespace

std::vector<uint8> MaterialSerializer::Serialize(const MaterialCookData& data) const
{
    if (data.parameters.size() > std::numeric_limits<uint32>::max())
        return {};

    std::vector<uint8> bytes;

    WriteUInt32(bytes, MaterialCookMagic);
    WriteUInt32(bytes, MaterialCookVersion);
    WriteString(bytes, data.shaderUUID.ToString());
    WriteUInt32(bytes, static_cast<uint32>(data.parameters.size()));

    for (const MaterialCookParameter& parameter : data.parameters)
    {
        if (parameter.name.size() > std::numeric_limits<uint32>::max())
            return {};

        WriteString(bytes, parameter.name);
        bytes.push_back(static_cast<uint8>(parameter.type));

        switch (parameter.type)
        {
        case MaterialCookParameterType::Float:
        {
            const float* value = std::get_if<float>(&parameter.value);
            if (!value)
                return {};

            WriteFloat(bytes, *value);
            break;
        }
        case MaterialCookParameterType::Float2:
        {
            const Vector2* value = std::get_if<Vector2>(&parameter.value);
            if (!value)
                return {};

            WriteFloat(bytes, value->x);
            WriteFloat(bytes, value->y);
            break;
        }
        case MaterialCookParameterType::Float3:
        {
            const Vector3* value = std::get_if<Vector3>(&parameter.value);
            if (!value)
                return {};

            WriteFloat(bytes, value->x);
            WriteFloat(bytes, value->y);
            WriteFloat(bytes, value->z);
            break;
        }
        case MaterialCookParameterType::Float4:
        {
            const Color* value = std::get_if<Color>(&parameter.value);
            if (!value)
                return {};

            WriteFloat(bytes, value->r);
            WriteFloat(bytes, value->g);
            WriteFloat(bytes, value->b);
            WriteFloat(bytes, value->a);
            break;
        }
        case MaterialCookParameterType::Texture2D:
        {
            const VirtualPath* value = std::get_if<VirtualPath>(&parameter.value);
            if (!value || value->ToString().size() > std::numeric_limits<uint32>::max())
                return {};

            WriteString(bytes, value->ToString());
            break;
        }
        default:
            return {};
        }
    }

    return bytes;
}

bool MaterialSerializer::Deserialize(
    const std::vector<uint8>& bytes,
    MaterialCookData& data) const
{
    size_t offset = 0;
    uint32 magic = 0;
    uint32 version = 0;
    if (!ReadUInt32(bytes, offset, magic) ||
        !ReadUInt32(bytes, offset, version) ||
        magic != MaterialCookMagic ||
        version != MaterialCookVersion)
        return false;

    MaterialCookData result = {};
    std::string shaderUUID;
    uint32 parameterCount = 0;
    if (!ReadString(bytes, offset, shaderUUID) ||
        !ReadUInt32(bytes, offset, parameterCount) ||
        parameterCount > bytes.size() - offset)
        return false;

    result.shaderUUID = UUID::FromString(shaderUUID);
    result.parameters.reserve(parameterCount);

    std::unordered_set<std::string> parameterNames;
    for (uint32 i = 0; i < parameterCount; ++i)
    {
        MaterialCookParameter parameter = {};
        uint8 type = 0;

        if (!ReadString(bytes, offset, parameter.name) ||
            offset >= bytes.size())
        {
            return false;
        }

        type = bytes[offset++];
        if (parameter.name.empty() ||
            !IsValidParameterType(type) ||
            !parameterNames.insert(parameter.name).second)
        {
            return false;
        }

        parameter.type = static_cast<MaterialCookParameterType>(type);
        switch (parameter.type)
        {
        case MaterialCookParameterType::Float:
        {
            float value = 0.0f;
            if (!ReadFloat(bytes, offset, value))
                return false;
            parameter.value = value;
            break;
        }
        case MaterialCookParameterType::Float2:
        {
            Vector2 value = {};
            if (!ReadFloat(bytes, offset, value.x) ||
                !ReadFloat(bytes, offset, value.y))
            {
                return false;
            }
            parameter.value = value;
            break;
        }
        case MaterialCookParameterType::Float3:
        {
            Vector3 value = {};
            if (!ReadFloat(bytes, offset, value.x) ||
                !ReadFloat(bytes, offset, value.y) ||
                !ReadFloat(bytes, offset, value.z))
            {
                return false;
            }
            parameter.value = value;
            break;
        }
        case MaterialCookParameterType::Float4:
        {
            Color value = {};
            if (!ReadFloat(bytes, offset, value.r) ||
                !ReadFloat(bytes, offset, value.g) ||
                !ReadFloat(bytes, offset, value.b) ||
                !ReadFloat(bytes, offset, value.a))
            {
                return false;
            }
            parameter.value = value;
            break;
        }
        case MaterialCookParameterType::Texture2D:
        {
            std::string path;
            if (!ReadString(bytes, offset, path))
                return false;
            parameter.value = VirtualPath(path);
            break;
        }
        default:
            return false;
        }

        result.parameters.push_back(std::move(parameter));
    }

    if (offset != bytes.size())
        return false;

    data = std::move(result);
    return true;
}

void MaterialSerializer::WriteUInt32(std::vector<uint8>& bytes, uint32 value) const
{
    for (int32 shift = 0; shift < 32; shift += 8)
        bytes.push_back(static_cast<uint8>(value >> shift));
}

bool MaterialSerializer::ReadUInt32(
    const std::vector<uint8>& bytes,
    size_t& offset,
    uint32& value) const
{
    if (bytes.size() - offset < sizeof(uint32))
        return false;

    value = 0;
    for (size_t i = 0; i < sizeof(uint32); ++i)
        value |= static_cast<uint32>(bytes[offset + i]) << (i * 8);

    offset += sizeof(uint32);
    return true;
}

void MaterialSerializer::WriteFloat(std::vector<uint8>& bytes, float value) const
{
    WriteUInt32(bytes, std::bit_cast<uint32>(value));
}

bool MaterialSerializer::ReadFloat(
    const std::vector<uint8>& bytes,
    size_t& offset,
    float& value) const
{
    uint32 bits = 0;
    if (!ReadUInt32(bytes, offset, bits))
        return false;

    value = std::bit_cast<float>(bits);
    return true;
}

void MaterialSerializer::WriteString(std::vector<uint8>& bytes, const std::string& value) const
{
    WriteUInt32(bytes, static_cast<uint32>(value.size()));
    bytes.insert(bytes.end(), value.begin(), value.end());
}

bool MaterialSerializer::ReadString(
    const std::vector<uint8>& bytes,
    size_t& offset,
    std::string& value) const
{
    uint32 length = 0;
    if (!ReadUInt32(bytes, offset, length) || length > bytes.size() - offset)
        return false;

    value.assign(bytes.begin() + offset, bytes.begin() + offset + length);
    offset += length;
    return true;
}

} // namespace URay
