#pragma once

#include <Core/File/VirtualFileSystem.h>
#include <Core/Ray/EventRay.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class Surface;
class VerticalLayout;
class Label;

class FileGridItem final : public Widget
{
public:
    explicit FileGridItem(VirtualFileEntry entry);

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    const VirtualFileEntry& GetEntry() const { return entry; }

protected:
    Vector2 OnMeasure() override;

    void OnUpdate(float deltaTime) override;

    std::optional<Rect> GetClipRect() const override { return rect; }

public:
    EventRay<const VirtualFileEntry&> onActivated;

private:
    VirtualFileEntry entry = {};

    Surface* surface = nullptr;
    VerticalLayout* layout = nullptr;
    Surface* thumbnailSurface = nullptr;
    Label* nameLabel = nullptr;

};

} // namespace URay
