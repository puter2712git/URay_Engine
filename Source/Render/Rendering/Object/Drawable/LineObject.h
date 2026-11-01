#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/Object/Drawable/DrawableObject.h>

#include <Core/Math/Color.h>
#include <Core/Math/Vector3.h>

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
    Vector<Line> lines;
};

class LineObject : public DrawableObject
{
    URAY_CLASS(LineObject, DrawableObject)

public:
    LineObject(const LineObjectState& state);
    ~LineObject();

public:
    void Update(const LineObjectState& state);
    void Submit(DrawCommandBuilder& builder, const DrawCommandParams& params) const override;

private:
    LineObjectState state;
};

} // namespace URay::Render
