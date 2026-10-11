#pragma once

#include <Core/File/VirtualFileSystem.h>
#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Widget.h>

namespace URay
{

class FileGridItem final : public Widget
{
public:
    explicit FileGridItem(VirtualFileEntry entry);

public:
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    const VirtualFileEntry& GetEntry() const { return entry; }
    EventRay<const VirtualFileEntry&>& GetOnActivatedRay() { return activatedRay; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    VirtualFileEntry entry = {};
    EventRay<const VirtualFileEntry&> activatedRay;
};

} // namespace URay
