#include "MaterialData.h"
#include <Core/Type/Types.h>

#include <Core/Serialization/YAMLConverters.h>

namespace URay
{

YAML::Node MaterialData::Serialize(YAML::Node& inNode) const
{
    YAML::Node node = inNode;
    node["Shader"] = shaderHandle;

    YAML::Node parametersNode;
    for (const auto& parameter : parameters)
    {
        YAML::Node parameterNode;
        parameterNode["Name"] = parameter.name;
        parameterNode["Type"] = static_cast<int>(parameter.type);

        switch (parameter.type)
        {
        case MaterialParameterType::Float:
            parameterNode["Value"] = std::get<float>(parameter.value);
            break;
        case MaterialParameterType::Float2:
            parameterNode["Value"] = std::get<Vector2>(parameter.value);
            break;
        case MaterialParameterType::Float3:
            parameterNode["Value"] = std::get<Vector3>(parameter.value);
            break;
        case MaterialParameterType::Float4:
            parameterNode["Value"] = std::get<Color>(parameter.value);
            break;
        case MaterialParameterType::Texture2D:
            parameterNode["Value"] = std::get<AssetHandle>(parameter.value);
            break;
        default:
            break;
        }

        parametersNode.push_back(parameterNode);
    }

    node["Parameters"] = parametersNode;

    return node;
}

void MaterialData::Deserialize(const YAML::Node& node)
{
    shaderHandle = AssetHandle::FromString(node["Shader"].as<String>());

    parameters.clear();

    const YAML::Node parametersNode = node["Parameters"];
    if (!parametersNode)
        return;

    parameters.reserve(parametersNode.size());

    for (const auto& parameterNode : parametersNode)
    {
        MaterialParameterData parameter;
        parameter.name = parameterNode["Name"].as<String>();
        parameter.type = static_cast<MaterialParameterType>(parameterNode["Type"].as<int>());

        const YAML::Node valueNode = parameterNode["Value"];

        switch (parameter.type)
        {
        case MaterialParameterType::Float:
            parameter.value = valueNode.as<float>();
            break;
        case MaterialParameterType::Float2:
            parameter.value = valueNode.as<Vector2>();
            break;
        case MaterialParameterType::Float3:
            parameter.value = valueNode.as<Vector3>();
            break;
        case MaterialParameterType::Float4:
            parameter.value = valueNode.as<Color>();
            break;
        case MaterialParameterType::Texture2D:
            parameter.value = valueNode.as<AssetHandle>();
            break;
        default:
            continue;
        }

        parameters.push_back(parameter);
    }
}

} // namespace URay
