#pragma once

#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/Color.h"
#include "Core/Math/Vector3.h"

namespace URay::Render
{

struct DirectionalLightObjectState
{
    Vector3 direction = Vector3::Forward;
    float intensity = 0.0f;
    Color color = Color::White;
};

class DirectionalLightObject : public RenderObject
{
public:
    DirectionalLightObject(const DirectionalLightObjectState& state);
    ~DirectionalLightObject() override;

public:
    void Update(const DirectionalLightObjectState& state);

private:
    Vector3 direction = Vector3::Forward;
    float intensity = 0.0f;
    Color color = Color::White;
};

} // namespace URay::Render
