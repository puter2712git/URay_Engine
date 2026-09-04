#include "Object.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Math/Vector4.h"
#include "Core/Type/Types.h"

namespace URay
{

URAY_REGISTER_CLASS(Object)

void Object::RegisterClass()
{
}

YAML::Node Object::Serialize() const
{
    YAML::Node node;
    const auto& properties = GetClass()->GetProperties();

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
        {
            const Vector2 value = prop.GetValue<Vector2>(this);
            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.x);
            valueNode.push_back(value.y);
            node[prop.name] = valueNode;
            break;
        }
        case PropertyType::Vector3:
        {
            const Vector3 value = prop.GetValue<Vector3>(this);
            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.x);
            valueNode.push_back(value.y);
            valueNode.push_back(value.z);
            node[prop.name] = valueNode;
            break;
        }
        case PropertyType::Vector4:
        {
            const Vector4 value = prop.GetValue<Vector4>(this);
            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.x);
            valueNode.push_back(value.y);
            valueNode.push_back(value.z);
            valueNode.push_back(value.w);
            node[prop.name] = valueNode;
            break;
        }
        case PropertyType::Color:
        {
            const Color value = prop.GetValue<Color>(this);
            YAML::Node valueNode(YAML::NodeType::Sequence);
            valueNode.push_back(value.r);
            valueNode.push_back(value.g);
            valueNode.push_back(value.b);
            valueNode.push_back(value.a);
            node[prop.name] = valueNode;
            break;
        }
        case PropertyType::String:
            node[prop.name] = prop.GetValue<std::string>(this);
            break;
        case PropertyType::Mesh:
        {
            Mesh* mesh = prop.GetValue<Mesh*>(this);
            node[prop.name] = mesh ? mesh->GetName() : "";
            break;
        }
        case PropertyType::Texture:
        {
            Texture* texture = prop.GetValue<Texture*>(this);
            node[prop.name] = texture->GetUUID().ToString();
            break;
        }
        default:
            break;
        }
    }

    return node;
}

void Object::Deserialize(const YAML::Node& node)
{
    const auto& properties = GetClass()->GetProperties();

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
        {
            Vector2& value = *static_cast<Vector2*>(valueAddress);
            value.x = valueNode[0].as<float>();
            value.y = valueNode[1].as<float>();
            break;
        }
        case PropertyType::Vector3:
        {
            Vector3& value = *static_cast<Vector3*>(valueAddress);
            value.x = valueNode[0].as<float>();
            value.y = valueNode[1].as<float>();
            value.z = valueNode[2].as<float>();
            break;
        }
        case PropertyType::Vector4:
        {
            Vector4& value = *static_cast<Vector4*>(valueAddress);
            value.x = valueNode[0].as<float>();
            value.y = valueNode[1].as<float>();
            value.z = valueNode[2].as<float>();
            value.w = valueNode[3].as<float>();
            break;
        }
        case PropertyType::Color:
        {
            Color& value = *static_cast<Color*>(valueAddress);
            value.r = valueNode[0].as<float>();
            value.g = valueNode[1].as<float>();
            value.b = valueNode[2].as<float>();
            value.a = valueNode[3].as<float>();
            break;
        }
        case PropertyType::String:
            *static_cast<std::string*>(valueAddress) = valueNode.as<std::string>();
            break;
        case PropertyType::Mesh:
        {
            const std::string assetName = valueNode.as<std::string>();
            *static_cast<Mesh**>(valueAddress) = assetName.empty()
                                                     ? nullptr
                                                     : gEngine->GetAssetSystem().FindMesh(assetName);
            break;
        }
        case PropertyType::Texture:
        {
            const UUID uuid = UUID::FromString(valueNode.as<std::string>());
            *static_cast<Texture**>(valueAddress) = gEngine->GetAssetSystem().Find<Texture>(uuid);
            break;
        }
        default:
            break;
        }

        if (prop.OnChangedCallback)
            prop.OnChangedCallback(this, prop);
    }
}

bool Object::IsA(Class* cls) const
{
    Class* self = GetClass();

    do
    {
        if (self == cls)
            return true;

        self = self->GetSuperClass();
    } while (self);

    return false;
}

} // namespace URay
