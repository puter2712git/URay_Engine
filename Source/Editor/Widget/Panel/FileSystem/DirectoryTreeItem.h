#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Widget.h>

namespace URay
{

class Surface;
class HorizontalLayout;
class Label;

class DirectoryTreeItem final : public Widget
{
public:
    DirectoryTreeItem(VirtualPath path, uint32 depth, bool expanded, bool selected);

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    EventRay<const VirtualPath&>& GetOnActivatedRay() { return activatedRay; }
    EventRay<const VirtualPath&>& GetOnExpandRequestedRay() { return expandRequestedRay; }

protected:
    Vector2 OnMeasure() override;
    void OnUpdate(float deltaTime) override;

private:
    VirtualPath path = {};
    uint32 depth = 0;
    bool expanded = false;
    bool selected = false;

    Surface* surface = nullptr;
    HorizontalLayout* layout = nullptr;
    Label* disclosureLabel = nullptr;
    Label* directoryLabel = nullptr;

    EventRay<const VirtualPath&> activatedRay;
    EventRay<const VirtualPath&> expandRequestedRay;
};

} // namespace URay
