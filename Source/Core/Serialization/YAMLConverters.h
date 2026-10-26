#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/Math/Color.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Core/Type/UUID.h>

#include <yaml-cpp/yaml.h>

namespace YAML
{

template <>
struct convert<URay::Vector2>
{
    static Node encode(const URay::Vector2& value)
    {
        Node node(NodeType::Sequence);
        node.push_back(value.x);
        node.push_back(value.y);
        return node;
    }

    static bool decode(const Node& node, URay::Vector2& value)
    {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        value.x = node[0].as<float>();
        value.y = node[1].as<float>();
        return true;
    }
};

template <>
struct convert<URay::Vector3>
{
    static Node encode(const URay::Vector3& value)
    {
        Node node(NodeType::Sequence);
        node.push_back(value.x);
        node.push_back(value.y);
        node.push_back(value.z);
        return node;
    }

    static bool decode(const Node& node, URay::Vector3& value)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        value.x = node[0].as<float>();
        value.y = node[1].as<float>();
        value.z = node[2].as<float>();
        return true;
    }
};

template <>
struct convert<URay::Vector4>
{
    static Node encode(const URay::Vector4& value)
    {
        Node node(NodeType::Sequence);
        node.push_back(value.x);
        node.push_back(value.y);
        node.push_back(value.z);
        node.push_back(value.w);
        return node;
    }

    static bool decode(const Node& node, URay::Vector4& value)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        value.x = node[0].as<float>();
        value.y = node[1].as<float>();
        value.z = node[2].as<float>();
        value.w = node[3].as<float>();
        return true;
    }
};

template <>
struct convert<URay::Color>
{
    static Node encode(const URay::Color& value)
    {
        Node node(NodeType::Sequence);
        node.push_back(value.r);
        node.push_back(value.g);
        node.push_back(value.b);
        node.push_back(value.a);
        return node;
    }
    static bool decode(const Node& node, URay::Color& value)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        value.r = node[0].as<float>();
        value.g = node[1].as<float>();
        value.b = node[2].as<float>();
        value.a = node[3].as<float>();
        return true;
    }
};

template <>
struct convert<URay::Quaternion>
{
    static Node encode(const URay::Quaternion& value)
    {
        Node node(NodeType::Sequence);
        node.push_back(value.x);
        node.push_back(value.y);
        node.push_back(value.z);
        node.push_back(value.w);
        return node;
    }

    static bool decode(const Node& node, URay::Quaternion& value)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        value.x = node[0].as<float>();
        value.y = node[1].as<float>();
        value.z = node[2].as<float>();
        value.w = node[3].as<float>();
        return true;
    }
};

template <>
struct convert<URay::UUID>
{
    static Node encode(const URay::UUID& value)
    {
        Node node(NodeType::Scalar);
        node = value.ToString();
        return node;
    }
    static bool decode(const Node& node, URay::UUID& value)
    {
        if (!node.IsScalar())
            return false;

        value = URay::UUID::FromString(node.as<URay::String>());
        return true;
    }
};

} // namespace YAML
