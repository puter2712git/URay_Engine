#include "Object.h"

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
            // Mesh instances currently have no persistent resource key to serialize.
            break;
        }
    }

    return node;
}

void Object::Deserialize(const YAML::Node& node)
{
}

} // namespace URay
