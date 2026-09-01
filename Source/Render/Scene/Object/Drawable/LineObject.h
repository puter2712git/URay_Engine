#pragma once

#include "Render/Scene/Object/Drawable/DrawableObject.h"

#include "Core/Math/Color.h"
#include "Core/Math/Vector3.h"

#include <vector>

namespace URay::Render
{

struct Line
{
    Vector3 start = Vector3::Zero;
    Vector3 end = Vector3::Zero;
    Color color = Color::White;
};

struct LineObjectState
{
    std::vector<Line> lines;
};

class LineObject : public DrawableObject
{
public:
    LineObject(const LineObjectState& state);
    ~LineObject();

public:
    void Update(const LineObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

private:
    std::vector<Line> lines;
};

} // namespace URay::Render
