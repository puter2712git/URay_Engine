#pragma once
#include <Core/Type/Types.h>

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/Material/MaterialParameter.h>

#include <yaml-cpp/yaml.h>

#include <vector>

namespace URay
{

struct MaterialParameterData
{
    String name;
    MaterialParameterType type;
    MaterialParameterValue value;
};

struct MaterialData
{
    AssetHandle shaderHandle;
    std::vector<MaterialParameterData> parameters;

    YAML::Node Serialize(YAML::Node& inNode) const;
    void Deserialize(const YAML::Node& node);
};

} // namespace URay
