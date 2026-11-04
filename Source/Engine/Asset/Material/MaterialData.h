#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/Material/MaterialParameter.h>
#include <Engine/Asset/Material/MaterialRenderMode.h>

#include <yaml-cpp/yaml.h>

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
    MaterialRenderMode renderMode = MaterialRenderMode::Opaque;
    Vector<MaterialParameterData> parameters;

    YAML::Node Serialize(YAML::Node& inNode) const;
    void Deserialize(const YAML::Node& node);
};

} // namespace URay
