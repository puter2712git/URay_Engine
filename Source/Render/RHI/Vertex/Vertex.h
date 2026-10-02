#pragma once

#include "Render/RHI/Vertex/VertexInputLayout.h"

#include "Core/Math/Color.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

struct Vertex
{
    Vector3 position;
    Vector2 uv;
    Color color;

    static const VertexInputLayout& GetInputLayout()
    {
        static const VertexInputLayout layout = []()
        {
            VertexInputLayout result;

            result.AddBinding(0, sizeof(Vertex));

            result.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
            result.AddAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv));
            result.AddAttribute(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color));

            return result;
        }();

        return layout;
    }
};

struct VertexUI
{
    Vector2 position;
    Vector2 uv;
    Color color;

    static const VertexInputLayout& GetInputLayout()
    {
        static const VertexInputLayout layout = []()
        {
            VertexInputLayout result;

            result.AddBinding(0, sizeof(VertexUI));

            result.AddAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexUI, position));
            result.AddAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexUI, uv));
            result.AddAttribute(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexUI, color));

            return result;
        }();

        return layout;
    }
};

struct VertexPNT
{
    Vector3 position = Vector3::Zero;
    Vector3 normal = Vector3::Up;
    Vector2 uv = Vector2::Zero;

    static const VertexInputLayout& GetInputLayout()
    {
        static const VertexInputLayout layout = []()
        {
            VertexInputLayout result;

            result.AddBinding(0, sizeof(VertexPNT));

            result.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPNT, position));
            result.AddAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPNT, normal));
            result.AddAttribute(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexPNT, uv));

            return result;
        }();

        return layout;
    }
};

} // namespace URay::Render
