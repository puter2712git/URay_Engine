#include "Object.h"

#include "Engine/Engine.h"
#include "Engine/Asset/Mesh/MeshAsset.h"
#include "Engine/Asset/Mesh/MeshManager.h"
#include "Engine/Asset/Texture/TextureAsset.h"
#include "Engine/Asset/Texture/TextureManager.h"

#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Math/Vector4.h"

namespace URay
{

URAY_REGISTER_CLASS(Object)

void Object::RegisterClass()
{
}

YAML::Node Object::Serialize()
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
        case PropertyType::String:
            node[prop.name] = prop.GetValue<std::string>(this);
            break;
        case PropertyType::Mesh:
            node[prop.name] = prop.GetValue<MeshAsset*>(this)->GetName();
            break;
        case PropertyType::Texture:
            node[prop.name] = prop.GetValue<TextureAsset*>(this)->GetName();
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

        void* valueAddress = reinterpret_cast<uint8_t*>(this) + prop.offset;

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
        case PropertyType::String:
            *static_cast<std::string*>(valueAddress) = valueNode.as<std::string>();
            break;
        case PropertyType::Mesh:
            *static_cast<MeshAsset**>(valueAddress) =
                gEngine->GetMeshManager()->GetMesh(valueNode.as<std::string>());
            break;
        case PropertyType::Texture:
            *static_cast<TextureAsset**>(valueAddress) =
                gEngine->GetTextureManager()->GetTexture(valueNode.as<std::string>());
            break;
        }

        if (prop.OnChangedCallback)
            prop.OnChangedCallback(this, prop);
    }
}

} // namespace URay
