#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Ray/EventRay.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class HorizontalLayout;

class BreadcrumbBar final : public Widget
{
public:
    BreadcrumbBar();

public:
    void Arrange(const Rect& rect) override;

    void SetPath(const VirtualPath& path);

    EventRay<const VirtualPath&>& GetOnPathActivatedRay() { return onPathActivated; }

protected:
    Vector2 OnMeasure() override;

private:
    void RebuildItems();

private:
    VirtualPath currentPath;

    HorizontalLayout* layout = nullptr;

    EventRay<const VirtualPath&> onPathActivated;
};

} // namespace URay
