#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Widget.h>

namespace URay
{

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

    void OnPaint(Render::UIDrawContext& context) override;

private:
    VirtualPath path = {};
    uint32 depth = 0;
    bool expanded = false;
    bool selected = false;

    Rect disclosureRect = {};
    EventRay<const VirtualPath&> activatedRay;
    EventRay<const VirtualPath&> expandRequestedRay;
};

} // namespace URay
