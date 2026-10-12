#include "ScriptComponent.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Script/ScriptAsset.h>
#include <Engine/Engine.h>
#include <Engine/Script/ScriptSystem.h>

#ifdef RegisterClass
#undef RegisterClass
#endif

namespace URay
{

URAY_REGISTER_CLASS(ScriptComponent)
URAY_REGISTER_COMPONENT(ScriptComponent)

void ScriptComponent::RegisterClass()
{
}

YAML::Node ScriptComponent::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["Script"] = scriptHandle.ToString();

    YAML::Node fieldsNode(YAML::NodeType::Map);

    for (const ScriptFieldValue& field : fields)
    {
        switch (field.type)
        {
        case PropertyType::Bool:
            fieldsNode[field.name] = std::get<bool>(field.value);
            break;

        case PropertyType::Int:
            fieldsNode[field.name] = std::get<int32>(field.value);
            break;

        case PropertyType::Float:
            fieldsNode[field.name] = std::get<float>(field.value);
            break;

        case PropertyType::String:
            fieldsNode[field.name] = std::get<std::string>(field.value);
            break;

        case PropertyType::Vector2:
        {
            const Vector2 value = std::get<Vector2>(field.value);

            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.x);
            valueNode.push_back(value.y);

            fieldsNode[field.name] = valueNode;
            break;
        }

        case PropertyType::Vector3:
        {
            const Vector3 value = std::get<Vector3>(field.value);

            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.x);
            valueNode.push_back(value.y);
            valueNode.push_back(value.z);

            fieldsNode[field.name] = valueNode;
            break;
        }

        case PropertyType::Vector4:
        {
            const Vector4 value = std::get<Vector4>(field.value);

            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.x);
            valueNode.push_back(value.y);
            valueNode.push_back(value.z);
            valueNode.push_back(value.w);

            fieldsNode[field.name] = valueNode;
            break;
        }

        case PropertyType::Color:
        {
            const Color value = std::get<Color>(field.value);

            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.r);
            valueNode.push_back(value.g);
            valueNode.push_back(value.b);
            valueNode.push_back(value.a);

            fieldsNode[field.name] = valueNode;
            break;
        }

        default:
            break;
        }
    }

    node["Fields"] = fieldsNode;
    return node;
}

void ScriptComponent::Deserialize(const YAML::Node& node)
{
    Component::Deserialize(node);

    const YAML::Node scriptNode = node["Script"];

    if (!scriptNode)
    {
        SetScript(AssetHandle{});
        return;
    }

    SetScript(AssetHandle::FromString(scriptNode.as<std::string>()));

    const YAML::Node fieldsNode = node["Fields"];

    if (!fieldsNode || !fieldsNode.IsMap())
        return;

    for (ScriptFieldValue& field : fields)
    {
        const YAML::Node valueNode = fieldsNode[field.name];

        if (!valueNode)
            continue;

        switch (field.type)
        {
        case PropertyType::Bool:
            field.value = valueNode.as<bool>();
            break;

        case PropertyType::Int:
            field.value = valueNode.as<int32>();
            break;

        case PropertyType::Float:
            field.value = valueNode.as<float>();
            break;

        case PropertyType::String:
            field.value = valueNode.as<std::string>();
            break;

        case PropertyType::Vector2:
        {
            if (!valueNode.IsSequence() || valueNode.size() != 2)
                continue;

            field.value = Vector2(
                valueNode[0].as<float>(),
                valueNode[1].as<float>());
            break;
        }

        case PropertyType::Vector3:
        {
            if (!valueNode.IsSequence() || valueNode.size() != 3)
                continue;

            field.value = Vector3(
                valueNode[0].as<float>(),
                valueNode[1].as<float>(),
                valueNode[2].as<float>());
            break;
        }

        case PropertyType::Vector4:
        {
            if (!valueNode.IsSequence() || valueNode.size() != 4)
                continue;

            field.value = Vector4(
                valueNode[0].as<float>(),
                valueNode[1].as<float>(),
                valueNode[2].as<float>(),
                valueNode[3].as<float>());
            break;
        }

        case PropertyType::Color:
        {
            if (!valueNode.IsSequence() || valueNode.size() != 4)
                continue;

            field.value = Color(
                valueNode[0].as<float>(),
                valueNode[1].as<float>(),
                valueNode[2].as<float>(),
                valueNode[3].as<float>());
            break;
        }

        default:
            break;
        }
    }
}

ScriptAsset* ScriptComponent::GetScript() const
{
    if (!scriptHandle.IsValid())
        return nullptr;

    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();

    return database.Find<ScriptAsset>(scriptHandle);
}

void ScriptComponent::SetScript(AssetHandle handle)
{
    if (scriptHandle == handle)
        return;

    scriptHandle = handle;
    RefreshFieldsFromSchema();
}

bool ScriptComponent::SetFieldValue(const std::string& name, const ScriptValue& value)
{
    for (ScriptFieldValue& field : fields)
    {
        if (field.name != name)
            continue;

        if (field.value.index() != value.index())
            return false;

        field.value = value;
        return true;
    }

    return false;
}

void ScriptComponent::RefreshFieldsFromSchema()
{
    ScriptAsset* script = GetScript();

    if (!script)
    {
        fields.clear();
        return;
    }

    const std::vector<ScriptFieldDefinition>& definitions = script->GetFields();

    std::vector<ScriptFieldValue> refreshedFields;
    refreshedFields.reserve(definitions.size());

    for (const ScriptFieldDefinition& definition : script->GetFields())
    {
        ScriptFieldValue value;
        value.name = definition.name;
        value.type = definition.type;
        value.value = definition.defaultValue;

        const auto existing = std::find_if(
            fields.begin(),
            fields.end(),
            [&definition](const ScriptFieldValue& field)
            {
                return field.name == definition.name &&
                       field.type == definition.type;
            });

        if (existing != fields.end())
            value.value = existing->value;

        refreshedFields.push_back(std::move(value));
    }

    fields = std::move(refreshedFields);
}

} // namespace URay
