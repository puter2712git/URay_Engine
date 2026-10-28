#pragma once

#include <Core/Math/Math.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Core/Type/TypeAlias.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Element/NumberField.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Widget.h>

#include <array>
#include <cstddef>

namespace URay
{

template <typename T>
struct VectorFieldTraits;

template <>
struct VectorFieldTraits<Vector2>
{
    static constexpr std::array members{ &Vector2::x, &Vector2::y };
};

template <>
struct VectorFieldTraits<Vector3>
{
    static constexpr std::array members{ &Vector3::x, &Vector3::y, &Vector3::z };
};

template <>
struct VectorFieldTraits<Vector4>
{
    static constexpr std::array members{ &Vector4::x, &Vector4::y, &Vector4::z, &Vector4::w };
};

template <typename T>
class VectorField final : public Widget, public RayEmitter
{
    using Traits = VectorFieldTraits<T>;
    static constexpr std::size_t ComponentCount = Traits::members.size();
    static constexpr float ComponentGap = 4.0f;
    static constexpr float LabelFieldGap = 4.0f;

    struct Component
    {
        Label* label = nullptr;
        NumberField<float>* field = nullptr;
    };

public:
    VectorField()
    {
        URAY_DECLARE_RAY(ValueChanged, T);
        constexpr std::array names{ "X", "Y", "Z", "W" };

        for (std::size_t i = 0; i < ComponentCount; ++i)
        {
            components[i].label = CreateEditorLabel(*this, names[i]);
            components[i].field = AddChild<NumberField<float>>();
            components[i].field->Register(
                "ValueChanged", this, &VectorField::OnComponentChanged, static_cast<int32>(i));
        }
    }

    void SetValue(const T& newValue)
    {
        value = newValue;
        for (std::size_t i = 0; i < ComponentCount; ++i)
            components[i].field->SetValue(value.*Traits::members[i]);
    }

    void Arrange(const Rect& rect) override
    {
        Widget::Arrange(rect);

        const float width = Math::Max(
            0.0f, (rect.size.x - ComponentGap * static_cast<float>(ComponentCount - 1)) /
                static_cast<float>(ComponentCount));
        Rect componentRect{ .position = rect.position, .size = Vector2(width, rect.size.y) };

        for (const Component& component : components)
        {
            const Vector2 labelSize = component.label->GetDesiredSize();
            component.label->Arrange({
                .position = Vector2(componentRect.position.x, componentRect.position.y + (rect.size.y - labelSize.y) * 0.5f),
                .size = labelSize,
            });
            component.field->Arrange({
                .position = Vector2(componentRect.position.x + labelSize.x + LabelFieldGap, componentRect.position.y),
                .size = Vector2(Math::Max(0.0f, width - labelSize.x - LabelFieldGap), rect.size.y),
            });
            componentRect.position.x += width + ComponentGap;
        }
    }

protected:
    Vector2 OnMeasure() override
    {
        float width = ComponentGap * static_cast<float>(ComponentCount - 1);
        float height = 0.0f;
        for (const Component& component : components)
        {
            const Vector2 labelSize = component.label->GetDesiredSize();
            const Vector2 fieldSize = component.field->GetDesiredSize();
            width += labelSize.x + LabelFieldGap + fieldSize.x;
            height = Math::Max({ height, labelSize.y, fieldSize.y });
        }
        return Vector2(width, height);
    }

private:
    void OnComponentChanged(float component, int32 index)
    {
        value.*Traits::members[static_cast<std::size_t>(index)] = component;
        Emit("ValueChanged", value);
    }

    T value{};
    std::array<Component, ComponentCount> components{};
};

} // namespace URay
