#include "ScriptComponent.h"

#include <Core/Log/LogSystem.h>
#include <Core/Math/Vector3.h>
#include <Core/Serialization/YAMLConverters.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Unit.h>
#include <Engine/Script/ScriptSystem.h>

#include <algorithm>
#include <utility>

namespace URay
{

namespace
{

template <typename T>
const T& GetOverrideValue(const ScriptFieldOverride& field)
{
    if (!field.value)
        throw std::runtime_error("Script field override value is missing.");

    const T* value = field.value->TryGet<T>();
    if (!value)
        throw std::runtime_error("Script field override type does not match.");

    return *value;
}

} // namespace

URAY_REGISTER_CLASS(ScriptComponent)
URAY_REGISTER_COMPONENT(ScriptComponent)

void ScriptComponent::RegisterClass()
{
    StaticClass()->AddProperty({ .type = PropertyType::String,
                                 .name = "Script Type",
                                 .offset = offsetof(ScriptComponent, scriptTypeName),
                                 .size = sizeof(String) });
}

const String& ScriptComponent::GetScriptTypeName() const
{
    return scriptTypeName;
}

void ScriptComponent::SetScriptTypeName(const String& typeName)
{
    scriptTypeName = typeName;
}

const Vector<ScriptFieldOverride>& ScriptComponent::GetFieldOverrides() const
{
    return fieldOverrides;
}

const ScriptFieldOverride* ScriptComponent::GetFieldOverride(const ScriptFieldInfo& field) const
{
    const auto it = std::find_if(fieldOverrides.begin(), fieldOverrides.end(),
                                 [&field](const ScriptFieldOverride& existing) {
        return existing.declaringTypeName == field.declaringTypeName &&
               existing.name == field.name &&
               existing.type == field.type;
    });

    if (it == fieldOverrides.end())
        return nullptr;

    return &(*it);
}

bool ScriptComponent::SetFieldOverride(
    const ScriptFieldInfo& field,
    std::optional<Variant> value)
{
    if (scriptTypeName.empty() || field.declaringTypeName.empty() || field.name.empty())
        return false;

    bool validValue = false;

    switch (field.type)
    {
    case ScriptFieldType::Bool:
        validValue = value && value->TryGet<bool>();
        break;

    case ScriptFieldType::Int:
        validValue = value && value->TryGet<int32>();
        break;

    case ScriptFieldType::Float:
        validValue = value && value->TryGet<float>();
        break;

    case ScriptFieldType::Vector3:
        validValue = value && value->TryGet<Vector3>();
        break;

    case ScriptFieldType::String:
        validValue = !value || value->TryGet<String>();
        break;

    default:
        break;
    }

    if (!validValue)
        return false;

    const auto it = std::find_if(fieldOverrides.begin(), fieldOverrides.end(),
                                 [&field](const ScriptFieldOverride& existing) {
        return existing.declaringTypeName == field.declaringTypeName &&
               existing.name == field.name &&
               existing.type == field.type;
    });

    if (it != fieldOverrides.end())
    {
        it->value = std::move(value);
        return true;
    }

    ScriptFieldOverride newOverride;
    newOverride.declaringTypeName = field.declaringTypeName;
    newOverride.name = field.name;
    newOverride.type = field.type;
    newOverride.value = std::move(value);

    fieldOverrides.push_back(std ::move(newOverride));
    return true;
}

YAML::Node ScriptComponent::Serialize() const
{
    YAML::Node node = Super::Serialize();

    YAML::Node overrides(YAML::NodeType::Sequence);

    for (const ScriptFieldOverride& field : fieldOverrides)
    {
        YAML::Node fieldNode;
        fieldNode["DeclaringType"] = field.declaringTypeName;
        fieldNode["Name"] = field.name;

        switch (field.type)
        {
        case ScriptFieldType::Bool:
            fieldNode["Type"] = "Bool";
            fieldNode["Value"] = GetOverrideValue<bool>(field);
            break;

        case ScriptFieldType::Int:
            fieldNode["Type"] = "Int";
            fieldNode["Value"] = GetOverrideValue<int32>(field);
            break;

        case ScriptFieldType::Float:
            fieldNode["Type"] = "Float";
            fieldNode["Value"] = GetOverrideValue<float>(field);
            break;

        case ScriptFieldType::Vector3:
            fieldNode["Type"] = "Vector3";
            fieldNode["Value"] = GetOverrideValue<Vector3>(field);
            break;

        case ScriptFieldType::String:
            fieldNode["Type"] = "String";

            if (field.value)
                fieldNode["Value"] = GetOverrideValue<String>(field);
            else
                fieldNode["Value"] = YAML::Node(YAML::NodeType::Null);

            break;

        default:
            URAY_LOG("[ScriptComponent] Unsupported script field override type: %d", static_cast<int32>(field.type));
        }

        overrides.push_back(fieldNode);
    }

    node["Fields"] = overrides;

    return node;
}

void ScriptComponent::Deserialize(const YAML::Node& node)
{
    fieldOverrides.clear();

    Super::Deserialize(node);

    const YAML::Node fieldsNode = node["Fields"];
    if (!fieldsNode.IsDefined())
        return;

    if (!fieldsNode.IsSequence())
    {
        URAY_LOG("[ScriptComponent] Fields must be sequence.");
        return;
    }

    for (const YAML::Node& fieldNode : fieldsNode)
    {
        try
        {
            if (!fieldNode.IsMap() ||
                !fieldNode["DeclaringType"].IsScalar() ||
                !fieldNode["Name"].IsScalar() ||
                !fieldNode["Type"].IsScalar() ||
                !fieldNode["Value"].IsDefined())
            {
                URAY_LOG("[ScriptComponent] Invalid script field entry.");
                continue;
            }

            ScriptFieldInfo field;
            field.declaringTypeName = fieldNode["DeclaringType"].as<String>();
            field.name = fieldNode["Name"].as<String>();

            const String typeName = fieldNode["Type"].as<String>();
            const YAML::Node valueNode = fieldNode["Value"];

            std::optional<Variant> value;

            if (typeName == "Bool")
            {
                field.type = ScriptFieldType::Bool;
                value.emplace(valueNode.as<bool>());
            }
            else if (typeName == "Int")
            {
                field.type = ScriptFieldType::Int;
                value.emplace(valueNode.as<int32>());
            }
            else if (typeName == "Float")
            {
                field.type = ScriptFieldType::Float;
                value.emplace(valueNode.as<float>());
            }
            else if (typeName == "Vector3")
            {
                field.type = ScriptFieldType::Vector3;
                value.emplace(valueNode.as<Vector3>());
            }
            else if (typeName == "String")
            {
                field.type = ScriptFieldType::String;
                if (!valueNode.IsNull())
                    value.emplace(valueNode.as<String>());
            }
            else
            {
                URAY_LOG("[ScriptComponent] Unsupported script field type: %s", typeName.c_str());
                continue;
            }

            if (!SetFieldOverride(field, std::move(value)))
            {
                URAY_LOG(
                    "[ScriptComponent] Failed to restore script field: %s",
                    field.name.c_str());
            }
        }
        catch (const YAML::Exception& exception)
        {
            URAY_LOG(
                "[ScriptComponent] Failed to read script field: %s",
                exception.what());
        }
    }
}

void ScriptComponent::OnLoad()
{
    updateFailed = false;

    if (scriptTypeName.empty())
    {
        URAY_LOG("[ScriptComponent] Script type is not set.");
        return;
    }

    Unit* ownerUnit = GetOwner();
    if (!ownerUnit)
    {
        URAY_LOG("[ScriptComponent] Owner unit is missing.");
        return;
    }

    instanceHandle = gEngine->GetScriptSystem().CreateScript(scriptTypeName.c_str(), ownerUnit->GetId(), fieldOverrides);

    if (instanceHandle == 0)
    {
        URAY_LOG("[ScriptComponent] Failed to create script: %s", scriptTypeName.c_str());
    }
}

void ScriptComponent::OnUpdate(float deltaTime)
{
    if (instanceHandle == 0 || updateFailed)
        return;

    const int32 result = gEngine->GetScriptSystem().UpdateScript(instanceHandle, deltaTime);

    if (result != 0)
    {
        URAY_LOG("[ScriptComponent] Script update failed: %d", result);
        updateFailed = true;
    }
}

void ScriptComponent::OnUnload()
{
    if (instanceHandle == 0)
        return;

    const int32 handle = instanceHandle;
    instanceHandle = 0;

    const int32 result = gEngine->GetScriptSystem().UnloadScript(handle);

    if (result != 0)
    {
        URAY_LOG("[ScriptComponent] Script unload failed: %d", result);
    }
}

} // namespace URay
