#pragma once

#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/Color.h"

namespace URay::Render
{

struct FogObjectState
{
    Color fogColor = Color::White;
    float fogStart = 0.0f;
    float fogEnd = 0.0f;
    float density = 0.0f;
};

class FogObject : public RenderObject
{
public:
    FogObject(const FogObjectState& state);
    ~FogObject() override;

public:
    void Update(const FogObjectState& state);

public:
    Color fogColor = Color::White;
    float fogStart = 0.0f;
    float fogEnd = 0.0f;
    float density = 0.0f;
};

} // namespace URay::Render
