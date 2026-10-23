#include "Object.h"

#include <Core/Math/Quaternion.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Core/Serialization/YAMLConverters.h>
#include <Core/Type/Types.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>

namespace URay
{

URAY_REGISTER_CLASS(Object)

void Object::RegisterClass() {}

YAML::Node Object::Serialize() const
{
    YAML::Node node;
    const auto& properties = GetClass()->GetAllProperties();

    for (const Property& prop : properties)
    {
        switch (prop.type)
        {
        case PropertyType::Bool:
            node[prop.name] = prop.GetValue<bool>(this);
            break;
        case PropertyType::Int:
            node[prop.name] = prop.GetValue<int>(this);
            break;
        case PropertyType::Float:
            node[prop.name] = prop.GetValue<float>(this);
            break;
        case PropertyType::Vector2:
            node[prop.name] = prop.GetValue<Vector2>(this);
            break;
        case PropertyType::Vector3:
            node[prop.name] = prop.GetValue<Vector3>(this);
            break;
        case PropertyType::Rotation:
            node[prop.name] = prop.GetValue<Quaternion>(this);
            break;
        case PropertyType::Vector4:
            node[prop.name] = prop.GetValue<Vector4>(this);
            break;
        case PropertyType::Color:
            node[prop.name] = prop.GetValue<Color>(this);
            break;
        case PropertyType::String:
            node[prop.name] = prop.GetValue<std::string>(this);
            break;
        case PropertyType::Mesh:
        case PropertyType::SkinnedMesh:
        case PropertyType::Texture:
        case PropertyType::Material:
            node[prop.name] = prop.GetValue<AssetHandle>(this).ToString();
            break;
        default:
            break;
        }
    }

    return node;
}

void Object::Deserialize(const YAML::Node& node)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    const auto& properties = GetClass()->GetAllProperties();

    for (const Property& prop : properties)
    {
        const YAML::Node valueNode = node[prop.name];
        if (!valueNode)
            continue;

        void* valueAddress = reinterpret_cast<uint8*>(this) + prop.offset;

        switch (prop.type)
        {
        case PropertyType::Bool:
            *static_cast<bool*>(valueAddress) = valueNode.as<bool>();
            break;
        case PropertyType::Int:
            *static_cast<int*>(valueAddress) = valueNode.as<int>();
            break;
        case PropertyType::Float:
            *static_cast<float*>(valueAddress) = valueNode.as<float>();
            break;
        case PropertyType::Vector2:
            *static_cast<Vector2*>(valueAddress) = valueNode.as<Vector2>();
            break;
        case PropertyType::Vector3:
            *static_cast<Vector3*>(valueAddress) = valueNode.as<Vector3>();
            break;
        case PropertyType::Rotation:
            *static_cast<Quaternion*>(valueAddress) = valueNode.as<Quaternion>();
            break;
        case PropertyType::Vector4:
            *static_cast<Vector4*>(valueAddress) = valueNode.as<Vector4>();
            break;
        case PropertyType::Color:
            *static_cast<Color*>(valueAddress) = valueNode.as<Color>();
            break;
        case PropertyType::String:
            *static_cast<std::string*>(valueAddress) = valueNode.as<std::string>();
            break;
        case PropertyType::Mesh:
        case PropertyType::SkinnedMesh:
        case PropertyType::Texture:
        case PropertyType::Material:
            *static_cast<AssetHandle*>(valueAddress) = valueNode.as<AssetHandle>();
            break;
        default:
            break;
        }

        NotifyPropertyChanged(prop);

        if (prop.OnChangedCallback)
            prop.OnChangedCallback(*this, prop);
    }
}

bool Object::IsA(const RuntimeType& type) const
{
    return IsTypeOf(GetRuntimeType(), type);
}

bool Object::IsA(Class* cls) const
{
    return cls && IsA(cls->GetType());
}

} // namespace URay
