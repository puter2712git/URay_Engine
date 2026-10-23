#pragma once

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/Material/MaterialParameter.h>

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>

namespace URay
{

struct MaterialParameterData
{
    std::string name;
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
